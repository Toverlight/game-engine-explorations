package engine.base;

import java.util.Locale;
import java.util.logging.Formatter;
import java.util.logging.LogRecord;
import java.text.SimpleDateFormat;
import java.util.Date;

public class CustomFormatter extends Formatter {

    // 定义日期格式：精确到毫秒
    private static final SimpleDateFormat dateFormat = new SimpleDateFormat("MMM dd, yyyy - HH:mm:ss::SSS >>> ",
            Locale.ENGLISH);

    // 获取调用者的类名和方法名
    private static String getCallerInfo() {
        StackTraceElement[] stackTrace = Thread.currentThread().getStackTrace();

        // 如果堆栈深度不足，返回简洁的调用信息
        if (stackTrace.length < 11) {
            return "UnknownCaller (stack trace too shallow, length: " + stackTrace.length + ")";
        }

        // 确定是否是通过 LogUtils 调用
        StackTraceElement caller;
        StackTraceElement upCaller;
        if (stackTrace[9].getClassName().equals(LogUtils.class.getName())) {
            caller = stackTrace[10]; // 如果 LogUtils 调用过，获取下一个调用者
            upCaller = stackTrace[11];
        } else {
            caller = stackTrace[9]; // 如果直接调用 Logger，获取当前调用者
            upCaller = null;
        }

        // 返回调用者的类名和方法名
        if (upCaller == null) {
            return caller.getClassName() + " " + caller.getMethodName();
        } else {
            return caller.getClassName() + " " + caller.getMethodName() + " <<< " + upCaller.getClassName() + " " +
                    upCaller.getMethodName() + " in " + upCaller.getFileName() + " line " +
                    upCaller.getLineNumber() + ":";
        }

    }

    @Override
    public String format(LogRecord record) {
        StringBuilder sb = new StringBuilder(180);

        // 格式化日期和时间
        String date = dateFormat.format(new Date(record.getMillis()));

        // 根据日志级别设置不同的颜色
        String level = record.getLevel().getName();
        String color = getColorForLevel(level);

//        // 获取类名、方法名等信息
        String source = getCallerInfo();

        // 格式化日志输出
        sb.append(color)
            .append(date)
            .append(source)
            .append("\n")
            .append(" [")
            .append(level)
            .append("] ")
            .append(record.getMessage())
            .append("\u001B[0m") // 关闭颜色（重置颜色）
            .append("\n");

        return sb.toString();
    }

    // 根据日志级别返回对应的颜色（ANSI 转义码）
    private String getColorForLevel(String level) {
        switch (level) {
            case "SEVERE":
                return "\u001B[31m"; // 红色
            case "WARNING":
                return "\u001B[33m"; // 黄色
            case "FINE":
            case "FINER":
            case "FINEST":
                return "\u001B[35m"; // 紫色
            case "INFO":
            case "CONFIG":
            default:
                return "\u001B[37m"; // 白色
        }
    }
}
