package project.inherent.structure.table;

/**
 * 通过「依据」找到多条记录的id
 * @param <T> 「依据」
 */
public interface FindIdsBy<T> {
    /**
     * 通过「依据」找到多条记录的id
     * @param ref 「依据」
     * @return id数组。若找不到，则返回new int[0]（空数组引用）
     */
    int[] findIdsBy(T ref);
}
