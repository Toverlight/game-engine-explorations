package xyz.topover.core.log;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class LogTest {
    private static final Logger logger = LoggerFactory.getLogger(LogTest.class);

    public static void main(String[] args) {
        logger.trace("这是 TRACE 级别日志");
        logger.debug("这是 DEBUG 级别日志");
        logger.info("这是 INFO 级别日志");
        logger.warn("这是 WARN 级别日志");
        logger.error("这是 ERROR 级别日志");

        // 模拟异常
        try {
            int result = 10 / 0;
        } catch (Exception e) {
            logger.error("发生异常:", e);
        }
    }
}
