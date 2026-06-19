-- Moves the GameObject with the WASD keys (during Play).
local Move = {}

Move.speed = 240.0   -- pixels per second

function Move:update(dt)
    local step = self.speed * dt / 1000.0
    if is_key_pressed(Key.W) then self.gameObject:translate(0, -step) end
    if is_key_pressed(Key.S) then self.gameObject:translate(0,  step) end
    if is_key_pressed(Key.A) then self.gameObject:translate(-step, 0) end
    if is_key_pressed(Key.D) then self.gameObject:translate( step, 0) end
end

return Move
