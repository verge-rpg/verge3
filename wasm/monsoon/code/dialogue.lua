vergeclass 'DialogueBox' do
    local INITIAL_PAUSE = 50
    local LETTER_PAUSE = 3
    local PAGE_PAUSE = 150
    local PAGE_SKIP_PAUSE = 220
    function DialogueBox:__init(param)
        self.messages = {}
        self.message_active = ''
        self.message_offset = 0
        self.wait = 0
        self.param = param
        self.width = math.floor(param.width * 0.375)
        self.desired_width = self.width
        self.width_increment = self.param.width_increment or 2
        
        self.lucency = 100
        self.desired_lucency = self.lucency
        self.message_lucency = self.lucency
        
        param.message_fade_in_step = param.message_fade_in_step or 1
        param.message_fade_out_step = param.message_fade_out_step or 1
        param.box_fade_in_step = param.box_fade_in_step or 1
        param.box_fade_out_step = param.box_fade_out_step or 1
    end
    
    function DialogueBox:AddMessage(message)
        if not self.param.no_wrap then
            message = font:WrapText(message, self.param.width - 16)
        end
        table.insert(self.messages, message)
    end
    
    function DialogueBox:SetMessage(message)
        if not self.param.no_wrap then
            message = font:WrapText(message, self.param.width - 16)
        end
        self.messages = { message }
        self:NextMessage()
        self.wait = 0
    end
    
    function DialogueBox:Render()
        if self.lucency < 100 then
            vx.SetLucent(self.lucency)
            box_style:DrawBox(self.param.x, self.param.y, self.param.x + self.width, self.param.y + 16 + self.param.height, vx.screen)
            vx.SetLucent(self.message_lucency)
            if self.width == self.desired_width and self.message_offset > 0 then
                if self.param.center then
                    font:PrintCenter(self.param.x + self.width / 2,
                        self.param.y + box_style.text_margin.y,
                        self.message_active:sub(1, self.message_offset))
                else
                    font:Print(self.param.x + box_style.text_margin.x,
                        self.param.y + box_style.text_margin.y,
                        self.message_active:sub(1, self.message_offset))
                end
            end
            vx.SetLucent(0)
        end
    end
    
    function DialogueBox:NextMessage()
        self.message_active = table.remove(self.messages, 1)
        self.message_offset = 0
        self.message_lucency = self.param.no_scroll and 100 or 0
    end
    
    function DialogueBox:Update() 
        if self.message_active == '' then
            if #self.messages ~= 0 then
                self:NextMessage()
            else
                self.desired_lucency = 100
                self.desired_width = math.floor(self.param.width * 0.375)
            end
        else
            self.desired_lucency = 50
            self.desired_width = self.param.width
        end
        
        if self.width < self.desired_width then
            if self.param.no_width_transition then
                self.width = self.desired_width
            else
                self.width = math.min(self.width + self.width_increment, self.desired_width)
            end
            self.message_offset = 0
            self.wait = self.param.initial_pause or INITIAL_PAUSE
        elseif self.width > self.desired_width then
            if not self.param.no_width_transition then
                self.width = math.max(self.width - 1, self.desired_width)
            end
        else
            if self.wait > 0 then
                self.wait = self.wait - 1

                if self.message_lucency > 0 then
                    self.message_lucency = math.max(self.message_lucency - self.param.message_fade_in_step, 0)
                end
            else
                if self.message_offset <= #self.message_active then
                    if self.param.no_scroll then
                        self.message_offset = #self.message_active + 1
                    else
                        self.message_offset = self.message_offset + 1
                    end
                    if self.message_offset >= #self.message_active then
                        self.wait = PAGE_PAUSE
                    else
                        self.wait = LETTER_PAUSE
                    end
                else
                    self.message_lucency = math.min(self.message_lucency + self.param.message_fade_out_step, 100)
                    if self.message_lucency == 100 then
                        self.message_active = ''
                    end
                end
            end
        end
        
        if self.lucency < self.desired_lucency then
            self.lucency = math.min(self.lucency + self.param.box_fade_out_step, self.desired_lucency)
        end
        if self.lucency > self.desired_lucency then
            self.lucency = math.max(self.lucency - self.param.box_fade_in_step, self.desired_lucency)
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
    
    function LocationBox:SetMessage(value, wait)
        self.text = value
        self.wait = wait
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
    local TEXTBOX_Y = 10
    
    local TEXTBOX_MINIMUM_LINES = 3
    
    local TEXTBOX_PRINT_DELAY = 2
    
    function ConfinedTextBox:__init()
    end

    local VOWELS = { 'A', 'E', 'I', 'O', 'U' }
    local CONSONANTS = { 'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z' }

    function ConfinedTextBox:DisplayText(text, target)
        if textbox_jumble then
            local len = #text
            local word = v3.Random(1, 10)
            local vowel_mod = v3.Random(1, 100) <= 50 and 0 or 1
            local t
            text = {}
            for i = 1, len do
                if word == 0 then
                    word = v3.Random(1, 10)
                    vowel_mod = v3.Random(1, 100) <= 50 and 0 or 1
                    text[i] = ' '
                else
                    word = word - 1
                    t = (i % 2 == vowel_mod) and VOWELS or CONSONANTS
                    text[i] = t[v3.Random(1, #t)]
                end
            end
            text[1] = '"'
            text[len] = '#'
            text = table.concat(text, '')
        end
    
        -- Subtract 16 because there's a bit o' padding on both sides
        local TEXTBOX_WIDTH = vx.screen.width - 10
        text = font:WrapText(text, TEXTBOX_WIDTH - 16)
        
        textbox_active = true
        
        local _, line_count = text:gsub('\n', '\n') -- v3.TokenCount(text, '\n') wouldn't count \n\n as two tokens. So gsub it is.
        line_count = math.max(line_count + 1, TEXTBOX_MINIMUM_LINES)
        
        local textbox_bottom = false
        if target and target.y - camera.y < TEXTBOX_Y + 16 + (line_count * font.height) then
            textbox_bottom = true
        end
        
        local box_time = vx.clock.systemtime + (#text * TEXTBOX_PRINT_DELAY)
        local done = false
        local mid_offset = 0
        local line_offset = 0
        local line_text = ""
        local img_arrow = resources.images.textbox_arrow
        
        button.Attack.pressed = false
        button.Jump.pressed = false
        vx.key.Enter.pressed = false
        
        local tx = TEXTBOX_X
        local ty = textbox_bottom and (vx.screen.height - 32 - TEXTBOX_Y - 16 - (line_count * font.height)) or TEXTBOX_Y
        
        while not done do
            vx.screen:RectFill(0, 0, vx.screen.width, vx.screen.height, vx.RGB(255, 0, 0))
            --vx.Render()
            Render()
            
            -- Draw our textbox window.
            old_style:DrawBox(tx, ty, tx + TEXTBOX_WIDTH, ty + 16 + (line_count * font.height), vx.screen)
            
            -- Draw a cute little text arrow.
            img_arrow:Blit(tx + TEXTBOX_WIDTH - 16, ty + (math.sin(math.rad(vx.clock.systemtime * 2)) * 4) + 6 + (line_count * font.height))
            
            -- Draw the lines of text.
            font_pink:Print(tx + 8, ty + 8, text:sub(1, mid_offset))
            vx.ShowPage()
            
            vx.UpdateControls()
            frame_limiter:Input()
            frame_limiter:Update()
            
            for i = 1, frame_limiter.gap do
                Update()
            end
            
            -- Increment the length our text midsection.
            if mid_offset < #text then
                mid_offset = math.min(#text - ((box_time - vx.clock.systemtime) / TEXTBOX_PRINT_DELAY), #text)
            end
            
            -- Pressing the accept button
            if button.Jump.pressed or button.Attack.pressed or vx.key.Enter.pressed then
                -- If we're waiting for text still, causes all text to appear.
                if mid_offset < #text then
                    mid_offset = #text				
                -- If we're at the end, exit this textbox loop.
                elseif mid_offset == #text then
                    done = true
                end
                
                button.Attack.pressed = false
                button.Jump.pressed = false
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

current_location_text = ''
current_song_name = 'silent'
current_song = nil
function dialogue.LocationText(message)
    local location_text = v3.GetToken(message, '|', 0)
    
    if v3.TokenCount(message, '|') > 1 then
        local s = v3.GetToken(message, '|', 1)
        
        if s == 'silent' and current_song then
            current_song:Stop()
            current_song_name = s
            current_song = nil
        else
            local song = _G[s]
            if song ~= current_song then
                current_song_name = s
                if song then
                    song:Play()
                end
                if current_song then 
                    current_song:Stop()
                end
                current_song = song
            end                
        end
    end
    
    if location_text ~= current_location_text then
        current_location_text = location_text
        location_box:SetMessage(location_text)
    end
end

function dialogue.InfoText(message)
    info_box:SetMessage(message)
end

function LocationZone(event)
    dialogue.LocationText(vx.map.zone[event.zone].name)
end

function dialogue.Wait(duration)
	local timestamp = vx.clock.systemtime + duration
    textbox_active = true
	while timestamp > vx.clock.systemtime do
		vx.screen:RectFill(0, 0, vx.screen.width, vx.screen.height, vx.RGB(255, 0, 0))
		Render()
        vx.ShowPage()
        
        vx.UpdateControls()
        frame_limiter:Input()
        frame_limiter:Update()
        
        for i = 1, frame_limiter.gap do
            Update()
        end
	end
    textbox_active = false
end

roaming_text_box = DialogueBox {
    x = 5;
    y = 5;
    width = vx.screen.width - 10;
    height = font.height * 3;
}

location_box = DialogueBox {
    x = 5;
    y = vx.screen.height / 2 - font.height / 2;
    width = vx.screen.width - 10;
    height = font.height;
    center = true;
    initial_pause = 0;
    box_fade_in_step = 3;
    box_fade_out_step = 1;
    message_fade_in_step = 2;
    message_fade_out_step = 1;
    no_width_transition = true;
    no_scroll = true;
    no_wrap = true;
}

info_box = DialogueBox {
    x = 5;
    y = vx.screen.height - font.height - 24;
    width = 10 + font:TextWidth('M') * 16;
    height = font.height;
    center = true;
    initial_pause = 0;
    box_fade_in_step = 2;
    box_fade_out_step = 2;
    message_fade_in_step = 2;
    message_fade_out_step = 2;
    no_width_transition = true;
    no_scroll = true;
    no_wrap = true;
}

confined_text_box = ConfinedTextBox()
