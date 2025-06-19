#pragma once

#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <iostream>

namespace logging {
    enum class LogLevel {
        Verbose = 0,
        Debug,
        Info,
        Warn,
        Error,
        Fatal,
        Silent
    };

    class Logger {
    public:
        explicit Logger(const std::string &name, const LogLevel logLevel = LogLevel::Debug) {
            const std::string pid = std::to_string(GetCurrentProcessId());

            this->name = colorize("[" + name + "]", WarnMeta);
            this->logLevel = logLevel;
            this->pid = pid + std::string(6 - pid.size(), ' ');
        }

        template<typename... T>
        void verbose(const T &... messages) const {
            this->log(VerboseMeta, messages...);
        }

        template<typename... T>
        void debug(const T &... messages) const {
            this->log(DebugMeta, messages...);
        }

        template<typename... T>
        void info(const T &... messages) const {
            this->log(InfoMeta, messages...);
        }

        template<typename... T>
        void warn(const T &... messages) const {
            this->log(WarnMeta, messages...);
        }

        template<typename... T>
        void error(const T &... messages) const {
            this->log(ErrorMeta, messages...);
        }

        template<typename... T>
        void fatal(const T &... messages) const {
            this->log(FatalMeta, messages...);
        }

        [[nodiscard]] LogLevel getLogLevel() const {
            return this->logLevel;
        }

        void setLogLevel(const LogLevel level) {
            this->logLevel = level;
        }

        ~Logger() = default;

    private:
        struct LogMeta {
            const LogLevel level;
            const char *label;
            const char *color;
        };

        static constexpr LogMeta VerboseMeta = {LogLevel::Verbose, "VERBOSE", "\033[96m"};
        static constexpr LogMeta DebugMeta = {LogLevel::Debug, "  DEBUG", "\033[95m"};
        static constexpr LogMeta InfoMeta = {LogLevel::Info, "   INFO", "\033[32m"};
        static constexpr LogMeta WarnMeta = {LogLevel::Warn, "   WARN", "\033[33m"};
        static constexpr LogMeta ErrorMeta = {LogLevel::Error, "  ERROR", "\033[31m"};
        static constexpr LogMeta FatalMeta = {LogLevel::Fatal, "  FATAL", "\033[1m"};
        static constexpr LogMeta SilentMeta = {LogLevel::Silent, " SILENT", "\033[0m"};

        std::string name;
        std::string pid;
        LogLevel logLevel;

        template<typename... T>
        void log(const LogMeta &meta, const T &... messages) const {
            if (this->logLevel > meta.level) {
                return;
            }

            std::ostringstream oss;
            (oss << ... << toString(messages));

            std::cout
                    << colorize("[SweepMiner]", meta)
                    << ' ' << colorize(this->pid, meta)
                    << " - " << timestamp()
                    << ' ' << colorize(meta.label, meta)
                    << ' ' << this->name
                    << ' ' << colorize(oss.str(), meta)
                    << std::endl;
        }

        template<typename T>
        static std::string toString(const T &value) {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }

        static std::string toString(const wchar_t *value) {
            if (!value) {
                return "";
            }

            const int32_t sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, value, -1, nullptr, 0, nullptr, nullptr);

            if (sizeNeeded <= 0) {
                return "";
            }

            std::string str(sizeNeeded - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, value, -1, str.data(), sizeNeeded, nullptr, nullptr);

            return str;
        }

        static std::string toString(WCHAR *value) {
            return toString(static_cast<const wchar_t *>(value));
        }

        static std::string colorize(const std::string &value, const LogMeta &meta) {
            return meta.color + value + SilentMeta.color;
        }

        static std::string timestamp() {
            const auto now = std::chrono::system_clock::now();
            const auto nowMs = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
            const auto epochSeconds = std::chrono::system_clock::to_time_t(now);
            const auto localTime = *std::localtime(&epochSeconds);
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(nowMs.time_since_epoch()).count() %
                            1000;

            std::ostringstream oss;

            oss << std::put_time(&localTime, "%Y-%m-%d %I:%M:%S")
                    << '.' << std::setw(3) << std::setfill('0') << ms
                    << ' ' << std::put_time(&localTime, "%p");

            return oss.str();
        }
    };
}
