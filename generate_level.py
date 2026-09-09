#!/usr/bin/env python3
"""
Parkour level generator.
Creates platforms, lights, shards and a spawn point, writing level.json.
"""

import json
import random
from collections import namedtuple

random.seed(42)

Vec3 = namedtuple("Vec3", ["x", "y", "z"])

PLATFORM_MATS = {
    "albedo": "assets/paved_albedo.png",
    "normal": "assets/paved_normal.png",
    "specular": "assets/full.png",
    "name": "basic",
}

LAMP_MATS = {
    "albedo": "assets/lamp.png",
    "normal": "assets/normal_none.png",
    "specular": "assets/none.png",
    "name": "basic",
}

SHARD_MESH = "assets/shard.obj"
ROUND_CUBE = "assets/round_cube.obj"
LAMP_MESH = "assets/lamp.obj"
PLATFORM_THICKNESS = 0.2


def platform(x, y, z, w, d, h=PLATFORM_THICKNESS):
    return {
        "mesh": ROUND_CUBE,
        "scale": {"x": w, "y": h, "z": d},
        "position": {"x": x, "y": y, "z": z},
        "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
        "collidable": True,
        "type": "NORMAL",
        "material": PLATFORM_MATS,
        "behavior": {"damage": 0},
    }


def lamp_post(x, y, z):
    return {
        "mesh": LAMP_MESH,
        "scale": {"x": 1.0, "y": 1.0, "z": 1.0},
        "position": {"x": x, "y": y, "z": z},
        "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
        "collidable": True,
        "type": "NORMAL",
        "material": LAMP_MATS,
        "behavior": {"damage": 0},
    }


def make_level():
    random.seed()
    meshes = []

    # ground
    meshes.append(platform(0, -10, 0, 100, 100, 1))

    # spawn: front of course
    spawn = {"position": {"x": 0.0, "y": -9.5, "z": 0.0}}

    # random walk: climb upward, lateral offsets, gaps
    path = []
    cx, cz = 0.0, 0.0
    top_y = -9.5
    used = set()
    used.add((round(cx, 2), round(cz, 2)))
    for i in range(random.randint(10, 18)):
        # random step size and lateral direction
        dx = random.choice([-1, 1]) * random.uniform(3.0, 7.0)
        dz = random.uniform(-3.0, 3.0)
        new_cx = cx + dx
        new_cz = cz + dz
        key = (round(new_cx, 2), round(new_cz, 2))
        # avoid drifting back exactly onto prior spot
        if key in used or abs(new_cx - cx) < 2.5:
            new_cx = cx + random.choice([-1, 1]) * random.uniform(3.5, 6.0)
            new_cz = cz + random.uniform(-2.0, 2.0)
        w = random.choice([2.0, 2.0, 2.5, 2.5, 3.0])
        d = random.choice([2.0, 2.0, 2.5, 2.5, 3.0])
        step_up = random.uniform(0.3, 1.0)
        top_y += step_up
        meshes.append(platform(new_cx, top_y, new_cz, w, d))

        # edge pillars for challenge
        height_gain = step_up + 0.2
        for nz in (new_cz + d / 2.0 + 0.2, new_cz - d / 2.0 - 0.2):
            if random.random() < 0.85:
                meshes.append(
                    {
                        "mesh": ROUND_CUBE,
                        "scale": {"x": 0.4, "y": max(0.6, height_gain + 0.3), "z": 0.4},
                        "position": {"x": new_cx, "y": top_y - 0.8 + 0.15, "z": nz},
                        "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
                        "collidable": True,
                        "type": "NORMAL",
                        "material": PLATFORM_MATS,
                        "behavior": {"damage": 0},
                    }
                )

        # lamps near each platform (random count)
        mesh_y = top_y + random.uniform(2.5, 4.0)
        for nx in (new_cx - w / 2.0 - 0.2, new_cx + w / 2.0 + 0.2):
            if random.random() < 0.75:
                meshes.append(lamp_post(nx, mesh_y, new_cz))

        cx, cz = new_cx, new_cz
        used.add(key)
        path.append((cx, top_y, cz, w, d))

    # side challenge platforms branching off path
    for _ in range(random.randint(3, 8)):
        base = random.choice(path)
        bcx, btop_y, bcz, bw, bd = base
        sx = bcx + random.choice([-1, 1]) * random.uniform(2.5, 8.0)
        sz = bcz + random.uniform(-6.0, 6.0)
        sw = random.choice([1.5, 1.5, 2.0, 2.0, 2.5])
        sd = random.choice([1.5, 1.5, 2.0, 2.0, 2.5])
        sy_top = btop_y - random.uniform(0.2, 1.2)
        meshes.append(platform(sx, sy_top, sz, sw, sd))
        # obstacle cube near it
        meshes.append(
            {
                "mesh": ROUND_CUBE,
                "scale": {
                    "x": random.choice([0.8, 1.0, 1.2]),
                    "y": random.uniform(1.5, 4.0),
                    "z": random.choice([0.8, 1.0, 1.2]),
                },
                "position": {
                    "x": sx + random.uniform(-0.3, 0.3),
                    "y": sy_top - random.uniform(0.6, 1.6),
                    "z": sz - sd / 2.0 - random.uniform(0.4, 1.0),
                },
                "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
                "collidable": True,
                "type": "NORMAL",
                "material": PLATFORM_MATS,
                "behavior": {"damage": 0},
            }
        )

    # ground-level scattered random platforms / cubes of various sizes
    for _ in range(random.randint(3, 6)):
        gx = random.uniform(-6.0, 6.0)
        gz = random.uniform(-8.0, 8.0)
        gw = random.uniform(1.5, 4.0)
        gd = random.uniform(1.5, 4.0)
        gy = -9.5 - random.uniform(0.0, 0.1)
        meshes.append(platform(gx, gy, gz, gw, gd))

    # lights
    lights = [
        {
            "position": {"x": 0.0, "y": 30.0, "z": 0.0},
            "color": {"x": 1.0, "y": 0.71, "z": 0.39},
            "radius": 140,
            "intensity": 8,
            "type": "POINT",
        },
    ]
    # accent lights on upper parts of the course
    upper = [p for p in path if p[1] > -3]
    for cx, top_y, cz, w, d in upper:
        if random.random() < 0.5:
            lights.append(
                {
                    "position": {
                        "x": cx,
                        "y": top_y + random.uniform(2.0, 5.0),
                        "z": cz,
                    },
                    "color": {
                        "x": random.uniform(0.5, 1.0),
                        "y": random.uniform(0.2, 0.7),
                        "z": random.uniform(0.2, 0.9),
                    },
                    "radius": random.randint(15, 30),
                    "intensity": random.randint(10, 16),
                    "type": "POINT",
                }
            )

    # shards scattered on upper platforms
    shards = []
    upper = [p for p in path if p[1] > -3]
    for cx, top_y, cz, w, d in upper:
        for _ in range(random.randint(1, 4)):
            sx = cx + random.uniform(-w / 2.0 + 0.3, w / 2.0 - 0.3)
            sz = cz + random.uniform(-d / 2.0 + 0.3, d / 2.0 - 0.3)
            shards.append(
                {"position": {"x": sx, "y": top_y + 0.3, "z": sz}}
            )

    return {"meshes": meshes, "spawn": spawn, "lights": lights, "shards": shards}



def main():
    data = make_level()
    with open("level.json", "w") as f:
        json.dump(data, f, indent=4)
    print(f"Wrote level.json with {len(data['meshes'])} meshes, "
          f"{len(data['lights'])} lights, {len(data['shards'])} shards.")


if __name__ == "__main__":
    main()
