package project.inherent;

import engine.base.LogUtils;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

/**
 * SQLite 数据库管理器。
 * 内部在运行时维持一个连接
 */
public class SQLiteManager {
    private static SQLiteManager instance;
    private Connection connection;
    private String url = "jdbc:sqlite:gamedata.db";

    private SQLiteManager() {
        connect();
    }

    /**
     * 获取实例
     */
    public static SQLiteManager getInstance() {
        if (instance == null) {// 第一次检查，如果实例未创建
            synchronized (SQLiteManager.class) {// 锁住整个类，确保多个线程不会同时执行此块代码
                if (instance == null) {// 第二次检查，防止多个线程竞争时已经创建了实例
                    instance = new SQLiteManager();// 创建实例
                }
            }
        }
        return instance;
    }

    /**
     * 获取连接
     */
    public Connection getConnection() {
        return connection;
    }

    /**
     * 关闭连接
     */
    public void close() {
        try {
            connection.close();
        } catch (SQLException e) {
            LogUtils.severe("Failed to close connection of 'gamedata.db',details below:\n" + e);
        }
    }

    /**
     * 创建新连接
     */
    public void connect() {
        try {
            connection = DriverManager.getConnection(url);
            LogUtils.info("Managed to connect to 'gamedata.db'.");
        } catch (SQLException e) {
            LogUtils.severe("Failed to connect to 'gamedata.db',details below:\n" + e);
        }
    }



    public static void main(String[] args) {
        SQLiteManager sqLiteManager = SQLiteManager.getInstance();
        Connection connection = sqLiteManager.getConnection();
        // 数据库操作

    }
}

