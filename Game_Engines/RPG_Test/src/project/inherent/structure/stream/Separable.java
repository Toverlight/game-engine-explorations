package project.inherent.structure.stream;

import java.util.List;

/**
 * 可分离的
 * @param <T> 结果集中元素类别
 */
public interface Separable<T> {
    /**
     * 以正则表达式匹配并分离，装至字符串列表
     * @param in 输入
     * @param regex 分离依据正则字符串
     * @return 结果列表
     */
    List<T> split(String in, String regex);
}
