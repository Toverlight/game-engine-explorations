package engine.base;

import project.inherent.GameRunner;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * 对JUL的优化包装
 */
public class LogUtils {

    /**
     * 默认日志（GameRunner中）是否可以输出该等级的日志
     */
    public static boolean isLoggable(Level level) {
        Logger logger = GameRunner.getLogger();
        return logger.isLoggable(level);
    }

    /**
     * 默认日志（GameRunner中）的等级是否与之相同
     */
    public static boolean isCurLevelEquals(Level level) {
        Logger logger = GameRunner.getLogger();
        return level.intValue() == logger.getLevel().intValue();
    }

    public static void severe(Logger logger, String message, Object... args) {
        if (logger.isLoggable(Level.SEVERE)) {
            logger.severe(String.format(message, args));
        }
    }
    public static void severe(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.isLoggable(Level.SEVERE)) {
            logger.severe(String.format(message, args));
        }
    }
    public static void warning(Logger logger, String message, Object... args) {
        if (logger.isLoggable(Level.WARNING)) {
            logger.warning(String.format(message, args));
        }
    }
    public static void warning(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.isLoggable(Level.WARNING)) {
            logger.warning(String.format(message, args));
        }
    }
    public static void info(Logger logger, String message, Object... args) {
        if (logger.isLoggable(Level.INFO)) {
            logger.info(String.format(message, args));
        }
    }
    public static void info(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.isLoggable(Level.INFO)) {
            logger.info(String.format(message, args));
        }
    }
    public static void config(Logger logger, String message, Object... args) {
        if (logger.isLoggable(Level.CONFIG)) {
            logger.config(String.format(message, args));
        }
    }
    public static void config(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.isLoggable(Level.CONFIG)) {
            logger.config(String.format(message, args));
        }
    }
    public static void fine(Logger logger, String message, Object... args) {
        if (logger.isLoggable(Level.FINE)) {
            logger.fine(String.format(message, args));
        }
    }
    public static void fine(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.isLoggable(Level.FINE)) {
            logger.fine(String.format(message, args));
        }
    }
    public static void fineOnly(Logger logger, String message, Object... args) {
        if (logger.getLevel() == Level.FINE) {
            logger.fine(String.format(message, args));
        }
    }
    public static void fineOnly(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.getLevel() == Level.FINE) {
            logger.fine(String.format(message, args));
        }
    }
    public static void finer(Logger logger, String message, Object... args) {
        if (logger.isLoggable(Level.FINER)) {
            logger.finer(String.format(message, args));
        }
    }
    public static void finer(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.isLoggable(Level.FINER)) {
            logger.finer(String.format(message, args));
        }
    }
    public static void finerOnly(Logger logger, String message, Object... args) {
        if (logger.getLevel() == Level.FINER) {
            logger.finer(String.format(message, args));
        }
    }
    public static void finerOnly(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.getLevel() == Level.FINER) {
            logger.finer(String.format(message, args));
        }
    }
    public static void finest(Logger logger, String message, Object... args) {
        if (logger.isLoggable(Level.FINEST)) {
            logger.finest(String.format(message, args));
        }
    }
    public static void finest(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.isLoggable(Level.FINEST)) {
            logger.finest(String.format(message, args));
        }
    }
    public static void finestOnly(Logger logger, String message, Object... args) {
        if (logger.getLevel() == Level.FINEST) {
            logger.finest(String.format(message, args));
        }
    }
    public static void finestOnly(String message, Object... args) {
        Logger logger = GameRunner.getLogger();
        if (logger.getLevel() == Level.FINEST) {
            logger.finest(String.format(message, args));
        }
    }

}
