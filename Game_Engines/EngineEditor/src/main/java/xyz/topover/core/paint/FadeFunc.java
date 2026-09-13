package xyz.topover.core.paint;

/**
 * <h1>衰减函数</h1>
 */
public interface FadeFunc {
    /**
     * 计算强度
     * @param len 最大照明半径
     * @param curLen 当前离光源的距离
     * @param halfAlpha 光源半张角大小
     * @param curHalfAlpha 当前与光线主方向的夹角
     * @param srcIntensity 光源强度
     * @return 当前位置的光照强度
     */
    double calcIntensity(double len, double curLen, double halfAlpha, double curHalfAlpha, double srcIntensity);
}
