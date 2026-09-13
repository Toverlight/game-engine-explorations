package engine.base.old;

/**
 * 基于帧的文本输出器
 */
public class FramePrinter {
    private int limit; // 每隔limit帧输出一次对应文本
    private String text;
    private int i;
    public FramePrinter(int limit, String constantText) {
        this.limit = limit;
        this.text = constantText;
        i = 0;
    }
    public FramePrinter(int limit) {
        this.limit = limit;
    }

    public void setLimit(int limit) {
        this.limit = limit;
    }

    public void setText(String text) {
        this.text = text;
    }

    public void Beat() {
        if (++i >= limit) {
            System.out.println(text);
            i = 0;
        }
    }

}
