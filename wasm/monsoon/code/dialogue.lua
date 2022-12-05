vergeclass 'RoamingTextBox' do
    local INITIAL_PAUSE = 50
    local LETTER_PAUSE = 3
    local PAGE_PAUSE = 150
    local PAGE_SKIP_PAUSE = 220
    function RoamingTextBox:__init()
        self.messages = {}
        self.message_active = ''
        self.message_offset = 0
        self.wait = 0
        self.width = 120
        self.desired_width = self.width
        
        self.lucency = 100
        self.desired_lucency = self.lucency
        self.message_lucency = self.lucency
    end
    
    function RoamingTextBox:AddMessage(message)
        message = font:WrapText(message, vx.screen.width - 26)
        table.insert(self.messages, message)
    end
    
    function RoamingTextBox:Render()
        if self.lucency < 100 then
            vx.SetLucent(self.lucency)
            box_style:DrawBox(5, 5, 5 + self.width, 5 + 16 + 3 * font.height, vx.screen)
            vx.SetLucent(self.message_lucency)
            if self.width == self.desired_width and self.message_offset > 0 then
                font:Print(5 + box_style.text_margin.x,
                    5 + box_style.text_margin.y,
                    self.message_active:sub(1, self.message_offset))
            end
            vx.SetLucent(0)
        end
    end
    
    function RoamingTextBox:Update() 
        if self.message_active == '' then
            if #self.messages ~= 0 then
                self.message_active = table.remove(self.messages, 1)
                self.message_offset = 0
                self.message_lucency = 0
            else
                self.desired_lucency = 100
                self.desired_width = 120
            end
        else
            self.desired_lucency = 50
            self.desired_width = vx.screen.width - 10
        end
        
        --[[if self.pause and (button.Attack.pressed or button.Jump.pressed or vx.key.Enter.pressed) then                
            if not self.skipped then
                self.message_offset = #self.message_active
                self.wait = PAGE_SKIP_PAUSE
                self.skipped = true
                self.width = self.desired_width
                self.lucency = self.desired_lucency

                button.Attack.pressed = false
                button.Jump.pressed = false
                vx.key.Enter.pressed = false
            else
                self.wait = 0
            end
        end]]
        
        if self.width < self.desired_width then
            self.width = self.width + 2
            self.message_offset = 0
            self.wait = INITIAL_PAUSE
        elseif self.width > self.desired_width then
            self.width = self.width - 1
        else            
            if self.wait > 0 then
                self.wait = self.wait - 1
            else
                if self.message_offset <= #self.message_active then
                    self.message_offset = self.message_offset + 1
                    if self.message_offset == #self.message_active then
                        self.wait = PAGE_PAUSE
                    else
                        self.wait = LETTER_PAUSE
                    end
                else
                    self.message_lucency = self.message_lucency + 1
                    if self.message_lucency >= 100 then
                        self.message_active = ''
                    end
                end
            end
        end
        if self.lucency < self.desired_lucency then
            self.lucency = self.lucency + 1
        end
        if self.lucency > self.desired_lucency then
            self.lucency = self.lucency - 1
        end
    end 
end

vergeclass 'LocationBox' do
    local TEXT_WAIT = 150
    function LocationBox:__init()
        self.lucency = 100
        self.text_lucency = self.lucency
        self.desired_lucency = self.lucency
        self.text = ""
        self.song = nil
        self.wait = 0
    end
    
    function LocationBox:SetText(value)
        local location_text = v3.GetToken(value, '|', 0)
        
        if v3.TokenCount(value, '|') > 1 then
            local s = v3.GetToken(value, '|', 1)
            
            if s == 'silent' and self.song then
                self.song:Stop()
            else
                local song = _G[s]
                if song and song ~= self.song then
                    song:Play()
                    if self.song then 
                        self.song:Stop()
                    end
                    self.song = song
                end                
            end
        end
        
        if self.text ~= location_text then
            self.text = location_text
            
            self.wait = TEXT_WAIT
        end
    end
    
    function LocationBox:Render()
        if self.lucency < 100 then
            vx.SetLucent(self.lucency)
            box_style:DrawBox(5, vx.screen.height / 2 - 24 - font.height / 2,
                vx.screen.width - 5, vx.screen.height / 2 - 8 + font.height / 2, vx.screen)
            vx.SetLucent(self.text_lucency)
            font:PrintCenter(vx.screen.width / 2,
                vx.screen.height / 2 - 24 - font.height / 2 + box_style.text_margin.y,
                self.text)
            vx.SetLucent(0)
        end
    end
    
    function LocationBox:Update()
        if self.wait > 0 then
            self.wait = self.wait - 1
            if self.wait <= TEXT_WAIT then
                self.desired_lucency = 50
            end
        else
            self.desired_lucency = 100
        end
        
        
        if self.lucency < self.desired_lucency then
            self.lucency = self.lucency + 1
            if self.text_lucency < 100 then
                self.text_lucency = self.text_lucency + 5
            end
        elseif self.lucency > self.desired_lucency then
            self.lucency = self.lucency - 5
        elseif self.text_lucency > 0 and self.wait > 0 then
            self.text_lucency = self.text_lucency - 2
        end
    end
end

vergeclass 'ConfinedTextBox' do
    local TEXTBOX_X = 5
    local TEXTBOX_Y = 5
    local TEXTBOX_MINIMUM_LINES = 3
    
    local TEXTBOX_PRINT_DELAY = 2
    
    function ConfinedTextBox:__init()
    end

    function ConfinedTextBox:DisplayText(text)
        -- Subtract 16 because there's a bit o' padding on both sides
        local TEXTBOX_WIDTH = vx.screen.width - 10
        text = font:WrapText(text, TEXTBOX_WIDTH - 16)
        
        textbox_active = true
        
        local line_count = math.max(v3.TokenCount(text, '\n'), TEXTBOX_MINIMUM_LINES)
        local box_time = vx.clock.systemtime + (#text * TEXTBOX_PRINT_DELAY)
        local done = false
        local mid_offset = 0
        local line_offset = 0
        local line_text = ""
        local img_arrow = resources.images.textbox_arrow
        
        button.Attack.pressed = false
        vx.key.Enter.pressed = false
        
        while not done do
            vx.screen:RectFill(0, 0, vx.screen.width, vx.screen.height, vx.RGB(255, 0, 0))
            vx.Render()
            
            -- Draw our textbox window.
            box_style:DrawBox(TEXTBOX_X, TEXTBOX_Y, TEXTBOX_X + TEXTBOX_WIDTH, TEXTBOX_Y + 16 + (line_count * font.height), vx.screen)
            
            -- Draw a cute little text arrow.
            img_arrow:Blit(TEXTBOX_X + TEXTBOX_WIDTH - 16, TEXTBOX_Y + (math.sin(math.rad(vx.clock.systemtime * 2)) * 4) + 6 + (line_count * font.height))
            
            -- Draw the lines of text.
            font:Print(TEXTBOX_X + 8, TEXTBOX_Y + 8, text:sub(1, mid_offset))
            vx.ShowPage()
            
            -- Increment the length our text midsection.
            if mid_offset < #text then
                mid_offset = math.min(#text - ((box_time - vx.clock.systemtime) / TEXTBOX_PRINT_DELAY), #text)
            end
            
            -- Pressing the accept button
            if button.Attack.pressed or vx.key.Enter.pressed then
                -- If we're waiting for text still, causes all text to appear.
                if mid_offset < #text then
                    mid_offset = #text				
                -- If we're at the end, exit this textbox loop.
                elseif mid_offset == #text then
                    done = true
                end
                
                button.Attack.pressed = false
                vx.key.Enter.pressed = false
            end
        end
        
        textbox_active = false
    end

end

dialogue = {}
function dialogue.ConfinedText(message)
    confined_text_box:DisplayText(message)
end

function dialogue.RoamingText(message)
    roaming_text_box:AddMessage(message)
end

function dialogue.LocationText(message)
    location_box:SetText(message)
end

function LocationZone(event)
    dialogue.LocationText(vx.map.zone[event.zone].name)
end

function dialogue.Wait(duration)
	local timestamp = vx.clock.systemtime + duration
	while timestamp > vx.clock.systemtime do
		vx.screen:RectFill(0, 0, vx.screen.width, vx.screen.height, vx.RGB(255, 0, 0))
		vx.Render()
		vx.ShowPage()
	end
end


roaming_text_box = RoamingTextBox()
confined_text_box = ConfinedTextBox()
location_box = LocationBox()
