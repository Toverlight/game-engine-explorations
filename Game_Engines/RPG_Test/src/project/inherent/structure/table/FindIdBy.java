package project.inherent.structure.table;

/**
 * 通过「依据」找到id
 * @param <T> 「依据」
 */
public interface FindIdBy<T> {
    /**
     * 通过「依据」找到id
     * @param ref 「依据」
     * @return id。若找不到，则返回-1
     */
    int findIdBy(T ref);
}
