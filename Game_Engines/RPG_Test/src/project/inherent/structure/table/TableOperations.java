package project.inherent.structure.table;

import java.util.List;

/**
 * 表基本操作
 * @param <T> 表数据项类型
 */
public interface TableOperations<T> {
    /**
     * 插入一条记录
     */
    void insert(T dataItem);

    /**
     * 更新一条记录
     */
    void update(T dataItem);

    /**
     * 删除一条记录
     */
    void delete(int id);

    /**
     * 查询某个记录。若找不到，返回null
     */
    T getById(int id);


    /**
     * 查询所有记录。若找不到，返回空链表
     */
    List<T> getAll();
}
