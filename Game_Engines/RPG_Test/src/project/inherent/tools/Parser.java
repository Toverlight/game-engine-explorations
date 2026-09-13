package project.inherent.tools;

/**
 * 解析工具（带默认值，不引发报错）
 */
public class Parser {
    public static int parseIntOrDefault(String str, int defaultValue) {
        try {
            return Integer.parseInt(str);  // 尝试解析字符串
        } catch (NumberFormatException e) {
            return defaultValue;  // 如果解析失败，返回默认值
        }
    }
}
