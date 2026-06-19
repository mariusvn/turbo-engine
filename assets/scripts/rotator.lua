-- Spins the GameObject. `speed` is editable in the inspector (degrees/second).
local Rotator = {}

Rotator.speed = 90.0

function Rotator:load()
    log("Rotator attached to", self.gameObject:get_name())
end

function Rotator:update(dt)
    -- dt is in milliseconds.
    self.gameObject:rotate(self.speed * dt / 1000.0)
end

return Rotator
