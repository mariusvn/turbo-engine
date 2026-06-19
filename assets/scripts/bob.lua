-- Oscillates the GameObject vertically around its starting position.
local Bob = {}

Bob.amplitude = 60.0   -- pixels
Bob.frequency = 1.0    -- cycles per second

function Bob:load()
    self.origin_y = self.gameObject:get_position().y
    self.elapsed = 0.0
end

function Bob:update(dt)
    self.elapsed = self.elapsed + dt / 1000.0
    local p = self.gameObject:get_position()
    local offset = math.sin(self.elapsed * self.frequency * 6.28318) * self.amplitude
    self.gameObject:set_position(p.x, self.origin_y + offset)
end

return Bob
