#include "LogFile.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>


// 负责追加到后端 和 写入到磁盘还是mmap
// openfd -> ftruncate -> mmap -> memcpy -> msync

// MMap类型
class MMapFileWriter : public FileWriter {
public:
    MMapFileWriter(const std::string &basename, uint32_t mem_size):
    mem_size_(mem_size)
    {
        std::cout << "24\n";
        writed_ = 0;
        if (fd_ >= 0) {
            close(fd_);
        }

        std::cout << "30\n";
        // 打开一个新文件 读写 | 不存在则创建它 | 用户读写 | 其他人只读
        fd_ = open(basename.c_str(), O_RDWR | O_CREAT, 
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        // 打开文件失败
        if (fd_ < 0) 
        {
            std::cout << "38\n";
            fprintf(stderr, "open new file failed,errno=%d", errno);
        } else {
            // ftruncate()会将参数fd指定的文件大小改为参数length指定的大小。
            int n = ftruncate(fd_, mem_size);
            // MAP_SHARED可以修改文件，进程间可见，回写到文件
            buffer_ = (char *)mmap(NULL, mem_size_, PROT_READ | PROT_WRITE,
                                    MAP_SHARED, fd_, 0);
            if (buffer_ == MAP_FAILED) {
                fprintf(stderr, "mmap file failed,errno=%d", errno);
                std::cout << "47\n";
            }
        }
    }


    ~MMapFileWriter() {
        if (fd_ >= 0) {
            close(fd_);
            fd_ = -1;
        }
        if (buffer_ != MAP_FAILED) {
            munmap(buffer_, mem_size_);
        }
    }


    // append信息
    void append(const char *msg, int32_t len) {
        if (len > mem_size_ - writed_) {
            //FIXME: 存在丢日志的风险
            fprintf(stderr, "mmap memory overflow ,errno=%d", errno);
            return;
        }
        memcpy(buffer_ + writed_, msg, len);
        writed_ += len;
    }
    // flush 全部持久化
    void flush() {
        if (buffer_ != MAP_FAILED) {
            msync(buffer_, mem_size_, MS_ASYNC);
        }
    }

    uint32_t writtenBytes() const { return writed_; }

private:
    int fd_;
    char *buffer_;
    int32_t mem_size_;
    int32_t writed_;
};

// 普通的文件类型
class AppendFileWriter : public FileWriter {
public:
    AppendFileWriter(const std::string &filename):
        fp_(::fopen(filename.c_str(), "ae")) 
    {
    std::cout << "96\n";
    ::setbuffer(fp_, buffer_, sizeof buffer_);
    }

    ~AppendFileWriter() {
        if (fp_) {
            ::fclose(fp_);
        }
    }

    void append(const char *msg, int32_t len) {
        size_t n = ::fwrite_unlocked(msg, 1, len, fp_);
        size_t remain = len - n;
        while (remain > 0) {
            size_t x = ::fwrite_unlocked(msg + n, 1, remain, fp_);
            if (x == 0) {
                int err = ferror(fp_);
                if (err) {
                    fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
                }
                break;
            }
            n += x;
            remain = len - n; // remain -= x
        }
        writen_ += len;
    }
    void flush() { ::fflush(fp_); }
    uint32_t writtenBytes() const { return writen_; }

private:
    FILE *fp_;
    char buffer_[64 * 1024];
    uint32_t writen_ = 0;
};

// 返回主机名
std::string getHostName() {
    // HOST_NAME_MAX 64
    // _POSIX_HOST_NAME_MAX 255
    char buf[256];
    if (::gethostname(buf, sizeof buf) == 0) {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    } else {
        return "unknownhost";
    }
}

std::string getLogFileName(const std::string &basename, time_t *now) {
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm); // FIXME: localtime_r ?
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += getHostName();

    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", getpid());
    filename += pidbuf;

    filename += ".log";

    return filename;
}

// LogFile对外接口
LogFile::LogFile(const std::string &basename, int32_t roll_size,
                 int32_t flush_interval, int32_t check_interval,
                 FileWriterType file_writer_type)
    : basename_(basename), 
        roll_size_(roll_size),
        flush_interval_(flush_interval), 
        check_freq_count_(check_interval),
        count_(0), 
        start_of_period_(0), 
        last_roll_(0), 
        last_flush_(0) 
{
    time_t now = 0;
    // 初始化文件名
    std::string filename = getLogFileName(basename_, &now);
    // 创建指向的文件指针
    if (file_writer_type == FileWriterType::MMAPFILE) {
        file_ = std::make_shared<MMapFileWriter>(filename, roll_size_);
    } else {
        file_ = std::make_shared<AppendFileWriter>(filename);
    }
    file_writer_type_ = file_writer_type;
    rollFile();
}

LogFile::~LogFile() = default;

void LogFile::append(const char *msg, int32_t len) {
    file_->append(msg, len);
    if (file_->writtenBytes() > roll_size_) {
        rollFile();
    } else {
        ++count_;
        //隔多久需要检查一下
        if (count_ >= check_freq_count_) {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t this_period = now / kRollPerSeconds * kRollPerSeconds;
            if (this_period != start_of_period_) {
                rollFile();
            } else if (now - last_flush_ > flush_interval_) {
                last_flush_ = now;
                file_->flush();
            }
        }
    }
}

void LogFile::flush() { file_->flush(); }

// 滚动日志，创建一个新的文件
bool LogFile::rollFile() {
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds * kRollPerSeconds;

    if (now > last_roll_) {
        last_roll_ = now;
        last_flush_ = now;
        start_of_period_ = start;
        if (file_writer_type_ == FileWriterType::MMAPFILE) {
            std::cout << "file_.reset(new MMapFileWriter(filename, roll_size_));\n"; 
            file_.reset(new MMapFileWriter(filename, roll_size_));
        } else {
            std::cout << "file_.reset(new AppendFileWriter(filename));\n"; 
            file_.reset(new AppendFileWriter(filename));
        }
    }
    return true;
}
