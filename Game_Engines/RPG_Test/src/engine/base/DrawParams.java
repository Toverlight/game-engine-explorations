package engine.base;

import javafx.scene.paint.Color;

/**
 * <h1>绘制参数简单类</h1>
 */
public class DrawParams {
    public Color strokeColor = Color.BLACK;// 描边颜色（设置为Color.TRANSPARENT则无描边）
    public Color fillColor = Color.WHITE;// 填充颜色（设置为Color.TRANSPARENT则无填充）
    public int lineWidth = 1;// 描边宽度（设置为0则无描边）
    public boolean isOpaque = true;// 是否不透明（是否绘制）

    public DrawParams() {}
    /**
     * <h1>绘制参数对象初始化</h1>
     * @param s 描边颜色
     * @param f 填充颜色
     * @param w 描边宽度
     * @param o 是否不透明
     */
    public DrawParams(Color s, Color f, int w, boolean o) {
        strokeColor = s;
        fillColor = f;
        lineWidth = w;
        isOpaque = o;
    }

    // --- 预设搭配 ---//
    public static final DrawParams BLACK_FILL = new DrawParams(Color.BLACK, Color.BLACK, 1, true);
    public static final DrawParams BLACK_STROKE = new DrawParams(Color.BLACK, Color.TRANSPARENT, 1, true);
    public static final DrawParams BLUE_FILL = new DrawParams(Color.BLUE, Color.BLUE, 1, true);
    public static final DrawParams BLUE_STROKE = new DrawParams(Color.BLUE, Color.TRANSPARENT, 1, true);
    public static final DrawParams RED_FILL = new DrawParams(Color.RED, Color.RED, 1, true);
    public static final DrawParams RED_STROKE = new DrawParams(Color.RED, Color.TRANSPARENT, 1, true);
    public static final DrawParams GREEN_FILL = new DrawParams(Color.GREEN, Color.GREEN, 1, true);
    public static final DrawParams GREEN_STROKE = new DrawParams(Color.GREEN, Color.TRANSPARENT, 1, true);
    public static final DrawParams YELLOW_FILL = new DrawParams(Color.YELLOW, Color.YELLOW, 1, true);
    public static final DrawParams YELLOW_STROKE = new DrawParams(Color.YELLOW, Color.TRANSPARENT, 1, true);

}
