package project.inherent.concrete.table;

import project.inherent.SQLiteManager;
import project.inherent.concrete.dataItem.TopDownTileMapItem;
import project.inherent.structure.table.FindIdBy;
import project.inherent.structure.table.TableOperations;

import java.sql.*;
import java.util.LinkedList;
import java.util.List;

public class TopDownTileMapTable implements TableOperations<TopDownTileMapItem>, FindIdBy<String> {
    private SQLiteManager sqLiteManager;

    // 通过构造器注入 SQLiteManager
    public TopDownTileMapTable(SQLiteManager sqLiteManager) {
        this.sqLiteManager = sqLiteManager;
    }


    @Override
    public void insert(TopDownTileMapItem dataItem) {
        String sql = "INSERT INTO TopDownTileMap (name) VALUES (?)";
        try (Connection conn = sqLiteManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            stmt.setString(1, dataItem.getName());
            stmt.executeUpdate();
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void update(TopDownTileMapItem dataItem) {
        String sql = "UPDATE TopDownTileMap SET name = ? WHERE id = ?";
        try (Connection conn = sqLiteManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            stmt.setString(1, dataItem.getName());
            stmt.setInt(2, dataItem.getId());
            stmt.executeUpdate();
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void delete(int id) {
        String sql = "DELETE FROM TopDownTileMap WHERE id = ?";
        try (Connection conn = sqLiteManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            stmt.setInt(1, id);
            stmt.executeUpdate();
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    @Override
    public TopDownTileMapItem getById(int id) {
        TopDownTileMapItem dataItem = null;
        String sql = "SELECT * FROM TopDownTileMap WHERE id = ?";
        try (Connection conn = sqLiteManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            stmt.setInt(1, id);
            ResultSet rs = stmt.executeQuery();
            if (rs.next()) {
                dataItem = new TopDownTileMapItem();
                dataItem.setId(id);
                dataItem.setName(rs.getString("name"));
                dataItem.setCreateDateTime(rs.getString("createDatetime"));
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return dataItem;
    }

    @Override
    public List<TopDownTileMapItem> getAll() {
        List<TopDownTileMapItem> dataItems = new LinkedList<>();
        String sql = "SELECT * FROM TopDownTileMap";
        try (Connection conn = sqLiteManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            ResultSet rs = stmt.executeQuery();
            while (rs.next()) {
                TopDownTileMapItem dataItem = new TopDownTileMapItem();
                dataItem.setId(rs.getInt("id"));
                dataItem.setName(rs.getString("name"));
                dataItem.setCreateDateTime(rs.getString("createDatetime"));
                dataItems.add(dataItem);
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return dataItems;
    }

    @Override
    public int findIdBy(String name) {
        String sql = "SELECT id FROM TopDownTileMap WHERE name = ?";
        try (Connection conn = sqLiteManager.getConnection();
             PreparedStatement stmt = conn.prepareStatement(sql)) {
            stmt.setString(1, name);
            ResultSet rs = stmt.executeQuery();
            if (rs.next()) {
                return rs.getInt("id");
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return -1;
    }
}
