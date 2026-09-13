package xyz.topover.core.entity;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;

public class PhysicsHandler {
    private static final List<Physics> physicsList = new ArrayList<>();

    private static final Logger logger = LoggerFactory.getLogger(PhysicsHandler.class);

    public static void registerPhysics(Physics physics) {
        physicsList.add(physics);
        logger.debug("Physics(id {}) registered.", physics.getId());
    }

    public static void unregisterPhysics(Physics physics) {
        physicsList.remove(physics);
        logger.debug("Physics(id {}) unregistered.", physics.getId());
    }

    public static void handleCollision() {
        for (int i = 0; i < physicsList.size() - 1; i++) {
            for (int j = i + 1; j < physicsList.size(); j++) {
                Physics.checkCollision(physicsList.get(i), physicsList.get(j));
            }
        }
    }

    public static void resetCollisionInfo() {
        for (Physics physics : physicsList) {
            physics.setCollisionInfo(null);
        }
    }
}
