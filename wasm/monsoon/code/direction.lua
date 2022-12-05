vergeclass 'Direction' do
	function Direction:__init(name, angle, opposite_key)
		self.name = name
		self.opposite_key = opposite_key
		self.angle = angle
	end
    
    Direction._property('opposite', function(self) return direction[self.opposite_key] end)
		
	function Direction:__tostring()
		return self.name
	end
end

direction = {
	Up = Direction('Up', 3 * math.pi / 2, 'Down'),
	Down = Direction('Down', math.pi / 2, 'Up'),
	Left = Direction('Left', math.pi, 'Right'),
	Right = Direction('Right', 0, 'Left')
}