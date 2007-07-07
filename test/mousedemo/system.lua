function autoexec()
	local imMouseCursor = LoadImage( "mouse-cursor.pcx" );
	--entities[3].tx = 9;
	while(true) do
		rectfill(0,0,320,240,makecolor(0,100,200),screen);
		printstring(0,0,screen,0,tostring(joy[0].button[2]));
		tblit(
			mouse.x,mouse.y,
			imMouseCursor,
			screen
		);
		
		showpage();
	end
end
