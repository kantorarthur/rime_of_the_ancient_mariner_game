import os
import sys

out = sys.argv[1]


def write_mtl(path, materials):
    lines = []
    for material in materials:
        name, kd, ks, diffuse_map, specular_map = material[:5]
        ke = material[5] if len(material) > 5 else None
        emissive_map = material[6] if len(material) > 6 else None
        lines.append("newmtl " + name)
        lines.append("Kd %.3f %.3f %.3f" % kd)
        lines.append("Ks %.3f %.3f %.3f" % ks)
        if diffuse_map:
            lines.append("map_Kd " + diffuse_map)
        if specular_map:
            lines.append("map_Ks " + specular_map)
        if ke:
            lines.append("Ke %.3f %.3f %.3f" % ke)
        if emissive_map:
            lines.append("map_Ke " + emissive_map)
        lines.append("")
    with open(os.path.join(out, path), "w") as f:
        f.write("\n".join(lines))


white = (1.0, 1.0, 1.0)
write_mtl("lighthouse/lighthouse.mtl", [("Material", white, white, "lighthouse_diff.jpg", "lighthouse_spec.png", None, "lighthouse_emissive.png")])
write_mtl("house/house.mtl", [("House20", white, (0.08, 0.08, 0.08), "house_diff.jpg", None)])
write_mtl("crate/crate.mtl", [("wooden_crate_02", white, white, "crate_diff.jpg", "crate_spec.png")])
write_mtl("lantern/lantern.mtl", [
    ("wooden_lantern_01", white, white, "lantern_diff.jpg", "lantern_spec.png"),
    ("wooden_lantern_01_glass", (1.0, 0.86, 0.55), (0.8, 0.8, 0.8), None, None, (1.0, 0.62, 0.25))])
for pier in ("pier/pier_a.mtl", "pier/pier_b.mtl"):
    write_mtl(pier, [
        ("modular_wooden_pier_planks", white, white, "planks_diff.jpg", "planks_spec.png"),
        ("modular_wooden_pier_poles", white, white, "poles_diff.jpg", "poles_spec.png")])
write_mtl("cliff/cliff.mtl", [("coastal_cliff_02", white, white, "cliff_diff.jpg", "cliff_spec.png")])
write_mtl("cliff/cliff_ring.mtl", [("coastal_cliff_02", white, white, "cliff_diff.jpg", "cliff_spec.png")])
write_mtl("rocks/rock_1.mtl", [("rock", white, white, "rock_dry_diff.jpg", "rock_dry_spec.png")])
write_mtl("rocks/rock_2.mtl", [("rock", white, white, "rock_dry_diff.jpg", "rock_dry_spec.png")])
write_mtl("rocks/rock_3.mtl", [("rock", white, white, "rock_dry_diff.jpg", "rock_dry_spec.png")])
write_mtl("island/island.mtl", [("ground", white, (0.05, 0.05, 0.05), "ground_diff.jpg", None)])
boat_colors = [
    ("Boat_White", (0.749, 0.749, 0.749), 0.25),
    ("Boat_Red", (0.379, 0.089, 0.084), 0.2),
    ("Boat_Gray", (0.099, 0.099, 0.099), 0.1),
    ("Metal", (0.404, 0.404, 0.404), 0.7),
    ("Wood", (0.416, 0.098, 0.0), 0.1),
    ("Plastic", (0.010, 0.010, 0.010), 0.4),
    ("Box_Blue", (0.061, 0.181, 0.326), 0.3),
    ("Box_Grey", (0.326, 0.326, 0.326), 0.3),
    ("Floater_Red", (0.8, 0.009, 0.0), 0.3),
    ("Floater_White", (0.8, 0.8, 0.8), 0.3),
    ("Yellow", (0.865, 0.421, 0.0), 0.3)]
write_mtl("boat/boat.mtl", [(name, kd, (ks, ks, ks), None, None) for name, kd, ks in boat_colors])
write_mtl("ship/ship.mtl", [
    ("dutch_ship_large_01_hull", white, white, "hull_diff.jpg", "hull_spec.png"),
    ("dutch_ship_large_01_rigging", white, (0.08, 0.08, 0.08), "rigging_diff.jpg", None),
    ("dutch_ship_large_01_sails", white, (0.05, 0.05, 0.05), "sails_diff.png", None)])
write_mtl("albatross/albatross.mtl", [("albatross", white, (0.1, 0.1, 0.1), "albatross_diff.png", None)])
print("materials written")
