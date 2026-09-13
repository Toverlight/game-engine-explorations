package xyz.topover.core.sql;

import org.jdbi.v3.core.Jdbi;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * <h1>SQLite 数据库管理器。</h1>
 * <p>
 *     基于jdbi.
 * </p>
 */
public class SQLiteManager {
    private static SQLiteManager instance;
    private Jdbi jdbi;
    private String dataBasePath = "db/";
    private String dataBaseName = "game_data";
    private String url = "jdbc:sqlite:" + dataBasePath + dataBaseName + ".db";
    private static final Logger logger = LoggerFactory.getLogger(SQLiteManager.class);

    private SQLiteManager() {
        connect();
    }

    /**
     * <h1>获取实例</h1>
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
     * <h1>创建新连接</h1>
     */
    public void connect() {
        try {
            jdbi = Jdbi.create(url);
            logger.info("Managed to connect to '{}.db'.", dataBaseName);
        } catch (Exception e) {
            logger.error("Failed to connect to '{}.db'. Error:{}", dataBaseName, e);
        }
    }

    /**
     * <h1>获取连接</h1>
     */
    public Jdbi getJdbi() {
        return jdbi;
    }

    /**
     * <h1>关闭连接</h1>
     */
    public void close() {
        jdbi = null;
        logger.info("Connection to '{}.db' closed.", dataBaseName);
    }

    /**
     * <h1>设置将要操作的数据库的名称</h1>
     * <p>
     *     设置完后自动连接。
     * </p>
     * @param dbName 数据库名称，不带后缀。若为null，则设为默认值 "game_data".
     */
    public void setDataBaseName(String dbName) {
        if (dbName == null) {
            dataBaseName = "game_data";
        } else {
            dataBaseName = dbName;
        }
        url = "jdbc:sqlite:" + dataBasePath + dataBaseName + ".db";
        connect();
    }

    /**
     * <h1>设置将要操作的数据库的路径</h1>
     * <p>
     *     路径若是相对路径，基于项目根路径。<br>
     *     <b>不要用'/'结尾。</b><br>
     *     设置完后自动连接。
     * </p>
     * @param dbPath 数据库路径，不带后缀。若为null，则设为默认值 "".
     */
    public void setDataBasePath(String dbPath) {
        if (dbPath == null) {
            dataBasePath = "db/";
        } else {
            dataBasePath = dbPath + "/";
        }
        url = "jdbc:sqlite:" + dataBasePath + dataBaseName + ".db";
        connect();
    }

    /**
     * <h1>设置将要操作的数据库的路径和名称</h1>
     * <p>
     *     路径若是相对路径，基于项目根路径。<br>
     *     <b>不要用'/'结尾。</b><br>
     *     设置完后自动连接。
     * </p>
     * @param dbPath 数据库路径，不带后缀。若为null，则设为默认值 "".
     * @param dbName 数据库名称，不带后缀。若为null，则设为默认值 "game_data".
     */
    public void setDataBasePathName(String dbPath, String dbName) {
        if (dbPath == null) {
            dataBasePath = "db/";
        } else {
            dataBasePath = dbPath + "/";
        }
        if (dbName == null) {
            dataBaseName = "game_data";
        } else {
            dataBaseName = dbName;
        }
        url = "jdbc:sqlite:" + dataBasePath + dataBaseName + ".db";
        connect();
    }

}

