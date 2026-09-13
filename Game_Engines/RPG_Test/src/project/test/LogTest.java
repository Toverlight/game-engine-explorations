package project.test;

import engine.base.CustomFormatter;
import sun.util.logging.resources.logging;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.logging.*;

public class LogTest {

    public static void main(String[] args) throws IOException {
        InputStream input = new FileInputStream("config/logging.properties");
        //取得日志管理器对象
        LogManager logManager = LogManager.getLogManager();
        //读取自定义的配置文件
        logManager.readConfiguration(input);

        Logger logger = Logger.getLogger(LogTest.class.getCanonicalName());
        Handler[] handlers = Logger.getLogger("").getHandlers();
        for (Handler handler : handlers) {
            System.out.println("Handler: " + handler.getClass().getName());
        }
        // 记录日志
        logger.severe("severe信息");
        logger.warning("warning信息");
        logger.info("info信息");
        logger.config("config信息");
        logger.fine("fine信息");
        logger.finer("finer信息");
        logger.finest("finest信息");
    }
}
