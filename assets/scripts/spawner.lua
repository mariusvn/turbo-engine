-- Demonstrates the extended API: spawns a ring of circles around this object.
local Spawner = {}

Spawner.count = 10
Spawner.radius = 140.0

function Spawner:load()
    local p = self.gameObject:get_position()
    for i = 1, self.count do
        local a = (i / self.count) * math.pi * 2.0
        local orb = spawn("orb_" .. i)
        orb:set_circle(14, 120, 200, 255)
        orb:set_position(p.x + math.cos(a) * self.radius,
                         p.y + math.sin(a) * self.radius)
    end
    log("Spawner created", self.count, "orbs around", self.gameObject:get_name())
end

return Spawner
