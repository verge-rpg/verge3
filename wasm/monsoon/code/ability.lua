vergeclass 'Ability' do
	local ABILITY_MAX_LEVEL = 4
	
	function Ability:__init(name, ability_info)
		self.name = ability_info['name'] or name
		if not ability_info['type'] then
			error('Failed to initialize ability because it has no type specified', 2)
		end
		self.description = ability_info['description'] or ''
		self.type = ability_info['type']
		self.discovered = false
		
		if ability_info['sprite_filename'] then
			self.sprite_filename = 'sprites/' .. ability_info['sprite_filename']
		end
		
		if self.type == 'attack' then
			local act = ability_info['action']
			if not act then
				error('Failed to initialize attack ability because the action behaviour was not defined', 2)
			end
			self.action = act

			local j = 1
			while act.projectile[tostring(j)] do
				local proj = act.projectile[tostring(j)]
				act.projectile[tostring(j)] = nil
				act.projectile[j] = proj

				if not proj.bullet or not bullets[proj.bullet] then
					error('Failed to initialize ability because bullet type "' .. proj.type .. '" does not exist.', 2)
				end
				proj.bullet = bullets[proj.bullet]
				proj.wait = tonumber(proj.wait)
				proj.x_offset = tonumber(proj.x_offset) or 0
				proj.y_offset = tonumber(proj.y_offset) or 0

				j = j + 1
			end

			if act.sound and resources.sounds[act.sound] then
				act.sound = resources.sounds[act.sound]
			end
			act.cooldown = tonumber(act.cooldown)
		end
	end	
end

abilities = nil
function InitAbilities()
	local ability_entries, err = MungoReadTable('resources/data/ability.mungo')
	if err then
		Exit(err)
	end
	
	abilities = {}
	for ability_name, ability_info in pairs(ability_entries) do
		abilities[ability_name] = Ability(ability_name, ability_info)
	end
end
