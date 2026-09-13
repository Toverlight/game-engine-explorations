package project.inherent.structure.stream;

/**
 * 文件流基本操作
 * @param <T> 输出类型
 * @param <E> 输入类型
 */
public interface StreamOperations<T, E> {
    /**
     * 读取。
     * <p>若无文件，规定要日志输出警告或错误</p>
     * @return 输出
     */
    T read();

    /**
     * 写入（覆写）。
     * <p>若无文件，则会先创建一个</p>
     * @param in 输入
     */
    void write(E in);

    /**
     * 文件是否可读写
     * @return 是否可读写
     */
    boolean isFileAccessible();
}
