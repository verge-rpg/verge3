vergeclass 'FrameLimiter' do
    local TIMER_MAX_FRAME_GAP = 2    
    local TIMER_SPEED_NORMAL, TIMER_SPEED_FAST_FORWARD, TIMER_SPEED_SLOW_MOTION = 1, 2, 3

    function FrameLimiter:__init()
        self.tick_last = vx.clock.systemtime
        self.second_last = 0
        self.gap = 0
        self.fractional_gap = 0
        self.update_count = 0
        self.frame_rate = 0
        self.speed = TIMER_SPEED_NORMAL
    end
        
    function FrameLimiter:Input()
        if button.FastForward.pressed then
            self.speed = TIMER_SPEED_FAST_FORWARD
        elseif button.SlowMotion.pressed then
            self.speed = TIMER_SPEED_SLOW_MOTION
        else
            self.speed = TIMER_SPEED_NORMAL
        end
    end
        
    function FrameLimiter:Update()
        self.update_count = self.update_count + 1
        if vx.clock.systemtime - self.second_last > 99 then
            self.frame_rate = self.update_count
            self.update_count = 0
            self.second_last = vx.clock.systemtime
        end
        if vx.clock.systemtime - self.tick_last > 0 then
            self.gap = vx.clock.systemtime - self.tick_last
            if self.fractional_gap >= 1 then
                self.gap = self.gap + math.floor(self.fractional_gap)
                self.fractional_gap = 0
            end
            if self.speed == TIMER_SPEED_FAST_FORWARD then
                self.gap = self.gap * 2.0
            elseif self.speed == TIMER_SPEED_FAST_FORWARD then
                self.gap = self.gap / 4.0
                self.fractional_gap = self.fractional_gap + (math.floor(self.gap) - self.gap)
            end
                
            if self.speed ~= TIMER_SPEED_FAST_FORWARD then
                self.gap = math.min(TIMER_MAX_FRAME_GAP, math.floor(self.gap))
            end
            self.tick_last = vx.clock.systemtime
        else
            self.gap = 0
        end
    end
end

frame_limiter = FrameLimiter()
