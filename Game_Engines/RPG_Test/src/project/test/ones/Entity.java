package project.test.ones;

import engine.base.old.Obj;
import engine.base.old.Shape2D;
import engine.base.old.Transform2D;
@Deprecated
public class Entity extends Obj {
    public Entity(Shape2D shape2D) {
        Transform2D transform2D = getTransform2D();
        transform2D.setShapeWithoutFlash(shape2D);
    }
}
