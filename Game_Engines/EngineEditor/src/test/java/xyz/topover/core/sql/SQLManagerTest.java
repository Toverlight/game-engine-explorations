package xyz.topover.core.sql;

import org.jdbi.v3.core.Jdbi;
import org.junit.jupiter.api.Test;

public class SQLManagerTest {
    @Test
    public void testJdbi() {
        SQLiteManager dbManager = SQLiteManager.getInstance();
        Jdbi jdbi = dbManager.getJdbi();

        jdbi.useHandle(handle -> {
            handle.execute("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
            handle.execute("SELECT * FROM users");
        });

        dbManager.close(); // 关闭数据库
    }
}
