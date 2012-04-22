function testkey()
	v3.log("you hit w key!");
	--v3.MapAlert("you hit w key!");
end

function testloop()
	local imMouseCursor = v3.LoadImage( "mouse-cursor.pcx" );

	while(true) do
		v3.rectfill(0,0,320,240,v3.makecolor(0,100,200),v3.screen);
		v3.printstring(0,0,v3.screen,0,tostring(v3.joy.up));
		--v3.PrintString(0,0,v3.screen,0,"Hello");
		v3.tblit(
			v3.mouse.x, v3.mouse.y,
			imMouseCursor,
			v3.screen
		);
		v3.showpage();
	end
	
end

local sfx = v3.LoadSound("slidewhistledown.wav");

function whizz()
	v3.log((v3.PlaySound(sfx,100)));
end

function spam()
	v3.StopMusic();
	--v3.StopSong(musics[1]); v3.StopSong(musics[2]); v3.StopSong(musics[3]); v3.StopSong(musics[4]); v3.StopSong(musics[5]);
	--if(lastpressed == v3.SCAN_1) then v3.PlaySong(musics[1]); end
	--if(lastpressed == v3.SCAN_2) then v3.PlaySong(musics[2]); end
	--if(lastpressed == v3.SCAN_3) then v3.PlaySong(musics[3]); end
	--if(lastpressed == v3.SCAN_4) then v3.PlaySong(musics[4]); end
	--if(lastpressed == v3.SCAN_5) then v3.PlaySong(musics[5]); end
end

function autoexec()
	musics = {}
--	musics[1] = LoadSong("cd-level1.it");
--	musics[2] = LoadSong("cd-level2.it");
--	musics[3] = LoadSong("cd-level3.it");
--	musics[4] = LoadSong("cd-level4.it");
--	musics[5] = LoadSong("cd-level5.it");
	
	--playmusic("cd-credits.it");
	v3.HookKey(v3.SCAN_W,"testkey");
	v3.HookKey(v3.SCAN_E,"whizz");
	v3.HookKey(v3.SCAN_1,"spam");
	v3.HookKey(v3.SCAN_2,"spam");
	v3.HookKey(v3.SCAN_3,"spam");
	v3.HookKey(v3.SCAN_4,"spam");
	v3.HookKey(v3.SCAN_5,"spam");
	--entities[3].tx = 9;
	v3.Map("bumville.map");
	
	testloop();
end
