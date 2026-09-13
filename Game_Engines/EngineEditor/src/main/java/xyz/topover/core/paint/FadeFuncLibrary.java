package xyz.topover.core.paint;

/**
 * <h1>光照衰减函数库</h1>
 */
public class FadeFuncLibrary {
    /**
     * <h1>线性衰减函数</h1>
     * <p>
     *     <b>与角ha, ch无关</b><br>
     *     自动限制输出在0~1之间。
     * </p>
     */
    public static final FadeFunc linear = (l, cl, ha, ch, s) -> Math.max(0, Math.min(1, - s * cl / l + s));
    /**
     * <h1>角相关线性衰减函数</h1>
     */
    public static final FadeFunc linearAngleRelated = (l, cl, ha, ch, s) -> (- s * cl / l + s) * (ha - ch) / ha;
}
