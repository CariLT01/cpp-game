import json
import random

def generate_random_level(num_platforms=20, output_file="generated_level.json"):
    meshes = []
    lights = []
    shards = []
    
    # 1. Base Spawn Platform and Lamp[cite: 1]
    spawn_position = {"x": 0.0, "y": -8.9, "z": 0.0}
    meshes.append({
        "mesh": "assets/round_cube.obj",
        "scale": {"x": 10, "y": 1, "z": 10},
        "position": {"x": 0.0, "y": -10.0, "z": 0.0},
        "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
        "collidable": True,
        "type": "NORMAL",
        "material": {
            "albedo": "assets/paved_albedo.png",
            "normal": "assets/paved_normal.png",
            "specular": "assets/full.png",
            "name": "basic"
        },
        "behavior": {"damage": 0}
    })
    
    meshes.append({
        "mesh": "assets/lamp.obj",
        "scale": {"x": 1, "y": 1, "z": 1},
        "position": {"x": 0.0, "y": -9.0, "z": 0.0},
        "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
        "collidable": True,
        "type": "NORMAL",
        "material": {
            "albedo": "assets/lamp.png",
            "normal": "assets/normal_none.png",
            "specular": "assets/none.png",
            "name": "basic"
        },
        "behavior": {"damage": 0}
    })

    # Ambient Light[cite: 1]
    lights.append({
        "position": {"x": 0.0, "y": 10.0, "z": 0.0},
        "color": {"x": 1.0, "y": 0.71, "z": 0.39},
        "radius": 75,
        "intensity": 5,
        "type": "POINT"
    })

    curr_x, curr_y, curr_z = 0.0, -9.5, 0.0

    MAX_FLAT_DIST = 20.0
    MIN_FLAT_DIST = 14.0

    # 2. Platform Generation with Controlled Heights
    for i in range(num_platforms):
        # Tighter vertical bounds (-3.0 to +4.0) to keep jump heights reasonable
        delta_y = random.uniform(-3.0, 4.0)
        
        # Horizontal penalty based on vertical offset
        vert_penalty = abs(delta_y) * 1.2
        min_x = max(10.0, MIN_FLAT_DIST - (vert_penalty * 0.5))
        max_x = max(12.0, MAX_FLAT_DIST - vert_penalty)
        
        delta_x = random.uniform(min_x, max_x)
        delta_z = random.uniform(-6.0, 6.0)
        
        curr_x += delta_x
        curr_y += delta_y
        curr_z += delta_z
        
        scale_x = random.uniform(2.0, 3.5)
        scale_z = random.uniform(2.0, 3.5)
        
        platform_pos = {
            "x": round(curr_x, 2),
            "y": round(curr_y, 2),
            "z": round(curr_z, 2)
        }
        
        meshes.append({
            "mesh": "assets/round_cube.obj",
            "scale": {"x": round(scale_x, 2), "y": 0.2, "z": round(scale_z, 2)},
            "position": platform_pos,
            "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
            "collidable": True,
            "type": "NORMAL",
            "material": {
                "albedo": "assets/paved_albedo.png",
                "normal": "assets/paved_normal.png",
                "specular": "assets/full.png",
                "name": "basic"
            },
            "behavior": {"damage": 0}
        })
        
        # 3. Guaranteed Light per platform[cite: 1]
        lights.append({
            "position": {
                "x": platform_pos["x"],
                "y": round(platform_pos["y"] + 3.0, 2),
                "z": platform_pos["z"]
            },
            "color": {
                "x": round(random.uniform(0.2, 1.0), 2),
                "y": round(random.uniform(0.2, 1.0), 2),
                "z": round(random.uniform(0.2, 1.0), 2)
            },
            "radius": random.randint(20, 40),
            "intensity": random.randint(6, 12),
            "type": "POINT"
        })
        
        # 4. Guaranteed Shard per platform[cite: 1]
        shards.append({
            "position": {
                "x": platform_pos["x"],
                "y": round(platform_pos["y"] + 1.5, 2),
                "z": platform_pos["z"]
            }
        })

    level_data = {
        "meshes": meshes,
        "spawn": {"position": spawn_position},
        "lights": lights,
        "shards": shards
    }
    
    with open(output_file, "w") as outfile:
        json.dump(level_data, outfile, indent=4)

if __name__ == "__main__":
    generate_random_level(num_platforms=20)