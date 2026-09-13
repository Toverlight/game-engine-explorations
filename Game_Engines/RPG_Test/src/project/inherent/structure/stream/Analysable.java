package project.inherent.structure.stream;

import java.util.List;

/**
 * 可分析的
 * @param <T> 分析结果集中元素类别
 * @param <E> 分析依据类
 */
public interface Analysable<T, E> {
    /**
     * 分析
     * @param ref 「依据」
     * @return 分析结果
     */
    List<T> analyse(E ref);
}
