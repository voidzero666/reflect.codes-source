#include "Variables.h"
#include "Hooks.h"
#include "aes256/aes256.hpp"
#include <sstream>
#include "GUI/element.h"
#include "config_.hpp"
#include "scripting.h"

void xor_crypt(ByteArray& vec) {
	for (auto& c : vec) {
		for (const auto& k : xs64_extp_key)
			c ^= k;
	}
}

void CConfig::ReadConfigs(LPCTSTR lpszFileName)
{
	ConfigList.push_back(lpszFileName);
}
void CConfig::ResetToDefault()
{
	// FF007EFF
	menu_colors::main_color  = color_t(0xFF007EFF);
	menu_colors::background1 = color_t(27, 31, 40);
	menu_colors::background2 = color_t(35, 39, 50);

	vars.ragebot.enable = false;
	vars.ragebot.onkey = false;
	vars.ragebot.key.key = 0;
	vars.ragebot.key.type = 0;
	vars.ragebot.autoscope = false;
	vars.ragebot.silent = true;
	vars.ragebot.fov = 180;
	vars.ragebot.autoshoot = false;
	vars.ragebot.resolver = false;
	vars.ragebot.dt_teleport = false;
	vars.ragebot.antiexploit = false;
	vars.ragebot.hideshot = false;
	/*vars.ragebot.shotrecord = false;
	vars.ragebot.shot_clr = color_t(255, 255, 255, 255);*/


	vars.ragebot.override_dmg.key = 0;
	vars.ragebot.override_dmg.state = false;
	vars.ragebot.override_dmg.type = 1;

	vars.ragebot.force_safepoint.key = 0;
	vars.ragebot.force_safepoint.state = false;
	vars.ragebot.force_safepoint.type = 1;

	vars.ragebot.double_tap.state = false;
	vars.ragebot.double_tap.key = 0;
	vars.ragebot.double_tap.type = 1;

	vars.ragebot.hideShots.state = false;
	vars.ragebot.hideShots.key = 0;
	vars.ragebot.hideShots.type = 1;

	for (auto& a : vars.ragebot.weapon) {
		a.enable = false;
		a.mindamage = 0;
		a.mindamage_override = 0;
		a.hitchance = 0;
		a.doubletap_hc = 0;
		a.multipoint = false;
		//a.quickstop = false;
		a.autostopMode = 0;
		a.static_scale = false;
		for (int l = 0; l < 10; l++)
			a.hitboxes[l] = false;
		a.scale_head = 0;
		a.scale_body = 0;
		a.prefer_safepoint = false;
	}
	
	vars.ragebot.dt_charge_delay = 0;
	vars.ragebot.active_index = 0;

	vars.antiaim.enable = false;
	vars.antiaim.shotDesync = false;
	vars.antiaim.aa_on_use = false;

	vars.antiaim.fakehead.state = false;
	vars.antiaim.fakehead.key = 0;
	vars.antiaim.fakehead.type = 1;
	
	vars.antiaim.fakelag = 0;
	vars.antiaim.fakelag_when_exploits = true;
	vars.antiaim.fakelag_when_standing = false;
	vars.antiaim.fakelagfactor = 1;
	vars.antiaim.fakelagvariance = 25;

	vars.antiaim.fakeduck.state = false;
	vars.antiaim.fakeduck.key = 0;
	vars.antiaim.fakeduck.type = 1;

	vars.antiaim.slowwalk.state = false;
	vars.antiaim.slowwalk.key = 0;
	vars.antiaim.slowwalk.type = 1;
	vars.antiaim.slowwalkspeed = 100;

	vars.antiaim.zlean = 0;
	vars.antiaim.zleanenable = false;

	vars.visuals.enable = false;
	vars.visuals.dormant = false;
	vars.visuals.skeleton = false;
	vars.visuals.skeleton_color = color_t(255, 255, 255, 255);
	vars.visuals.box = false;
	vars.visuals.box_color = color_t(255, 255, 255, 255);
	vars.visuals.healthbar = false;
	vars.visuals.override_hp = false;
	vars.visuals.hp_color = color_t(0, 255, 0, 255);
	vars.visuals.weapon = false;
	vars.visuals.weapon_color = color_t(255, 255, 255, 255);
	vars.visuals.name = false;
	vars.visuals.name_color = color_t(255, 255, 255, 255);
	vars.visuals.ammo = false;
	vars.visuals.ammo_color = color_t(255, 255, 255, 255);
	vars.visuals.flags = 0;
	vars.visuals.flags_color = color_t(255, 255, 255, 255);
	vars.visuals.out_of_fov = false;
	vars.visuals.out_of_fov_color = color_t(255, 255, 255, 255);
	vars.visuals.antiaim_arrows_color = color_t(255, 255, 255, 255);
	vars.visuals.hitmarker = false;
	vars.visuals.hitmarker_sound = false;
	vars.visuals.hitmarker_sound_type = 0;
	vars.visuals.hitmarker_color = color_t(255, 100, 100, 255);
	vars.visuals.speclist = false;
	vars.visuals.monitor = false;
	vars.visuals.nadepred = false;
	vars.visuals.nadepred_color = color_t(255, 255, 255, 255);
	vars.visuals.taser_range = 0;
	vars.visuals.taser_range_color = color_t(255, 255, 255, 255);

	vars.misc.peek_assist.state = false;
	vars.misc.peek_assist.key = 0;
	vars.misc.peek_assist.type = 0;

	vars.misc.thirdperson.state = false;
	vars.misc.thirdperson.key = 0;
	vars.misc.thirdperson.type = 2;
	vars.misc.autostrafeonkey = false;
	vars.misc.autostrafekey.key = false;
	vars.misc.autostrafekey.state = false;
	vars.misc.autostrafekey.type = false;


	vars.visuals.thirdperson_dist = 150;
	vars.visuals.aspect_ratio = 0;
	vars.visuals.indicators = 0;
	vars.visuals.indicators_rage = false;
	vars.visuals.eventlog = false;
	vars.visuals.eventlog_color = color_t(255, 255, 255, 255);
	vars.visuals.bullet_tracer = false;
	vars.visuals.bullet_tracer_type = 0;
	vars.visuals.bullet_tracer_color = color_t(255, 0, 0, 255);
	vars.visuals.bullet_impact = false;
	vars.visuals.bullet_impact_color = color_t(0, 0, 255, 127);
	vars.visuals.impacts_size = 2.0f;
	vars.visuals.bullet_tracer_local_color = color_t(0, 255, 0, 255);
	for (int i = 0; i < 8; i++)
		vars.visuals.remove[i] = false;
	vars.visuals.nightmode = false;
	vars.visuals.customize_color = false;
	vars.visuals.nightmode_amount = 50;
	vars.visuals.nightmode_color = color_t(43, 41, 46, 255);
	vars.visuals.nightmode_prop_color = color_t(255, 255, 255, 255);
	vars.visuals.nightmode_skybox_color = color_t(255, 255, 255, 255);
	vars.visuals.kill_effect = false;
	vars.misc.worldfov = 0;
	vars.misc.viewmodelfov = 68;
	vars.visuals.zeus_warning = false;

	vars.visuals.chams = false;
	vars.visuals.chamsxqz = false;
	vars.visuals.chams_ragdoll = false;
	vars.visuals.ragdoll_force = false;
	vars.visuals.glow = false;
	vars.visuals.glowtype = 0;
	vars.visuals.local_glow = false;
	vars.visuals.glow_color = color_t(255, 255, 255, 255);
	vars.visuals.local_glow_clr = color_t(255, 255, 255, 255);
	vars.visuals.chamscolor = color_t(255, 255, 255, 255);
	vars.visuals.chamscolor_xqz = color_t(255, 255, 255, 255);
	vars.visuals.glow_col = color_t(255, 255, 255, 0);
	vars.visuals.glow_col_xqz = color_t(255, 255, 255, 0);
	vars.visuals.chamstype = 0;
	vars.visuals.metallic_clr = color_t(255, 255, 255, 150);
	vars.visuals.metallic_clr2 = color_t(255, 255, 255, 255);
	vars.visuals.phong_exponent = 0;
	vars.visuals.phong_boost = 0;
	vars.visuals.rim = 0;
	vars.visuals.overlay = 0;
	vars.visuals.overlay_xqz = 0;
	vars.visuals.chams_brightness = 100;

	vars.visuals.localchams = false;
	vars.visuals.localchams_color = color_t(255, 255, 255, 255);
	vars.visuals.localchams_color_glow = color_t(255, 255, 255, 0);
	vars.visuals.localchamstype = 0;
	vars.visuals.local_chams.metallic_clr = color_t(255, 255, 255, 150);
	vars.visuals.local_chams.metallic_clr2 = color_t(255, 255, 255, 255);
	vars.visuals.local_chams.phong_exponent = 0;
	vars.visuals.local_chams.phong_boost = 0;
	vars.visuals.local_chams.rim = 0;
	vars.visuals.local_chams_brightness = 100.f;
	vars.visuals.local_chams.overlay = 0;
	vars.visuals.out_of_fov_distance = 100;
	vars.visuals.out_of_fov_size = 25;
	vars.visuals.out_of_fov_drawvisible = false;
	vars.visuals.out_of_fov_visiblecolor = color_t(255, 255, 255, 255);

	vars.visuals.interpolated_bt = true;
	vars.visuals.interpolated_dsy = true;

	for (int i = 0; i < 5; ++i) {
		vars.visuals.misc_chams[i].enable = false;
		vars.visuals.misc_chams[i].clr = color_t(255, 255, 255, 255);
		vars.visuals.misc_chams[i].glow_clr = color_t(255, 255, 255, 0);
		vars.visuals.misc_chams[i].material = 0;
		vars.visuals.misc_chams[i].metallic_clr = color_t(255, 255, 255, 150);
		vars.visuals.misc_chams[i].metallic_clr2 = color_t(255, 255, 255, 255);
		vars.visuals.misc_chams[i].phong_exponent = 0;
		vars.visuals.misc_chams[i].phong_boost = 0;
		vars.visuals.misc_chams[i].rim = 0;
		vars.visuals.misc_chams[i].chams_brightness = 100.f;
		vars.visuals.misc_chams[i].overlay = 0;
	}

	vars.visuals.recoil_crosshair = false;
	vars.visuals.radarhack = false;

	vars.visuals.world.weapons.enabled = false;
	vars.visuals.world.weapons.color = color_t(255, 255, 255, 255);

	vars.visuals.world.projectiles.enable = false;
	vars.visuals.world.projectiles.colors[0] = color_t(35, 255, 35, 255);
	vars.visuals.world.projectiles.colors[1] = color_t(255, 35, 35, 255);
	vars.visuals.world.projectiles.colors[2] = color_t(255, 255, 255, 255);
	vars.visuals.world.projectiles.filter[0] = false;
	vars.visuals.world.projectiles.filter[1] = false;
	vars.visuals.world.projectiles.filter[2] = false;
	vars.visuals.world.weapons.planted_bomb = false;
	vars.visuals.world.weapons.color = color_t(255, 0, 0, 255);
	vars.visuals.world.weapons.planted_bombheight = 0.2f;
#ifdef _DEBUG
	vars.visuals.world.projectiles.trajectories = false;
#endif

	vars.misc.antiuntrusted = true;
	vars.misc.ublockOrigin = false;
	vars.misc.bunnyhop = false;
	vars.misc.knifebot = false;
	vars.visuals.clantagspammer = false;
	vars.visuals.clantagtype = 1;
	memset(vars.visuals.clantagcustomname, 0, 32);
	vars.visuals.clantagrotating = false;
	vars.misc.enableviewmodel = false;
	vars.misc.viewmodel_x = 0.f;
	vars.misc.viewmodel_y = 0.f;
	vars.misc.viewmodel_z = 0.f;
	vars.misc.viewmodel_roll = 0.f;
	vars.misc.autoaccept = false;
	vars.misc.autoacceptdelay = 0.f;

	vars.misc.autobuy.enable = false;
	vars.misc.autobuy.main = 0;
	for (int i= 0; i<7;i++)
		vars.misc.autobuy.misc[i] = 0;

	vars.misc.autobuy.pistol = 0;

	vars.movement.edgebug.enabled = false;
	vars.movement.edgebug.key.key = 0;
	vars.movement.edgebug.lock = 0.f;
	vars.movement.edgebug.ticks = 0;
	vars.movement.edgejump = false;
	vars.movement.edgejumpkey.key = 0;
	vars.movement.jumpbug = false;
	vars.movement.jumpbugkey.key = false;
	vars.movement.autostrafetype = 0;
	vars.movement.dxtostrafe = 0;
	vars.movement.msl = false;
	vars.movement.mslx = 25.f;
	memset(vars.visuals.hitmarker_name, 0, 32);

	vars.legitbot.autopistol = false;
	vars.legitbot.aimbot = false;
	vars.legitbot.active_index = 0;
	vars.legitbot.backtrack = false;
	vars.legitbot.btlength = 0.2f;
	vars.legitbot.key.key = 0;
	vars.legitbot.key.type = 0;
	vars.legitbot.onkey = false;
	vars.legitbot.teammates = false;
	vars.legitbot.rcsstandalone = false;
	for (int i = 0; i < vars.legitbot.weapons.size(); i++)
	{
		vars.legitbot.weapons[i].aimbacktrack = false;
		vars.legitbot.weapons[i].aimlock = false;
		vars.legitbot.weapons[i].silent = false;
		vars.legitbot.weapons[i].enabled = false;
		vars.legitbot.weapons[i].fov = 0.f;
		for (int l = 0; l < 5; l++)
			vars.legitbot.weapons[i].hitboxes[l] = false;
		vars.legitbot.weapons[i].ignoreflash = false;
		vars.legitbot.weapons[i].ignoresmoke = false;
		vars.legitbot.weapons[i].rcsx = 0.f;
		vars.legitbot.weapons[i].rcsy = 0.f;
		vars.legitbot.weapons[i].scopedonly = false;
		vars.legitbot.weapons[i].silent = false;
		vars.legitbot.weapons[i].smooth = 1.f;
	}

	vars.movement.graph.enabled = false;
	vars.movement.graph.color = color_t(200, 200, 200, 255);
	vars.movement.graph.height = 1.f;
	vars.movement.graph.size = 64;
	vars.movement.graph.linewidth = 5;
	vars.movement.graph.textcolor = color_t(245, 245, 220, 255);
	vars.movement.graph.thickness = 1;
	vars.movement.graph.speed = 1.f;
	vars.movement.ebdetect.soundtype = 0;
	vars.movement.ebdetect.mastertoggle = false;
	for (int i = 0; i < 4; i++)
		vars.movement.ebdetect.enabled[i] = false;
	memset(vars.movement.ebdetect.sound_name, 0, 32);
	vars.movement.recorder.enabled = false;
	vars.movement.recorder.infowindow = false;
	for (auto& b : vars.movement.recorder.infowindowselected)
		b = false;
	vars.movement.recorder.smoothing = 1.f;
	vars.movement.recorder.maxrender = 0;
	vars.movement.recorder.clearrecord.key = 0;
	vars.movement.recorder.clearrecord.state = false;
	vars.movement.recorder.clearrecord.type = 0;
	vars.movement.recorder.startplayback.key = 0;
	vars.movement.recorder.startplayback.state = false;
	vars.movement.recorder.startplayback.type = 0;
	vars.movement.recorder.stopplayback.key = 0;
	vars.movement.recorder.stopplayback.state = false;
	vars.movement.recorder.stopplayback.type = 0;
	vars.movement.recorder.startrecord.key = 0;
	vars.movement.recorder.startrecord.state = false;
	vars.movement.recorder.startrecord.type = 0;
	vars.movement.recorder.stopplayback.key = 0;
	vars.movement.recorder.stopplayback.state = false;
	vars.movement.recorder.stopplayback.type = 0;
	vars.movement.recorder.infowindowx = 0.1f;
	vars.movement.recorder.infowindowy = 0.25f;
	vars.movement.recorder.lockva = true;
	vars.movement.recorder.startring = color_t(255, 0, 0, 200);
	vars.movement.recorder.endring = color_t(0, 0, 255, 200);
	vars.movement.recorder.linecolor = color_t(255, 255, 255, 200);
	vars.movement.recorder.textcolor = color_t(255, 255, 255, 200);
	vars.movement.recorder.ringsize = 30.f;
	vars.movement.fastduck = false;
	vars.movement.slidewalk = false;
	vars.visuals.print_votes = false;
	vars.visuals.print_votes_chat = 0;
	vars.movement.jumpbugtype = 0;
	vars.movement.lengthcheck = 4;
	vars.visuals.motionblur.enabled = false;
	vars.visuals.motionblur.fwd_enabled = false;
	vars.visuals.motionblur.falling_min = 10.f;
	vars.visuals.motionblur.falling_max = 20.f;
	vars.visuals.motionblur.falling_intensity = 1.f;
	vars.visuals.motionblur.rotation_intensity = 1.f;
	vars.visuals.motionblur.strength = 1.f;
	vars.visuals.dlights = false;
	vars.visuals.dlights_color = { 255,255,255,255 };
	vars.movement.graph.fadetype = 0;
	vars.movement.graph.speedtofade = 150.f;
	vars.movement.staticvelcolor = false;
	vars.movement.velcolor = { 255,255,255,255 };
	vars.movement.velocityfade = false;
	vars.movement.velfadevelocity = 250.f;
	vars.movement.edgebug.locktype = 0;
	vars.movement.edgebug.detect_strafe = false;
	vars.movement.edgebug.marker_color = { 255,255,255,255 };
	vars.movement.edgebug.marker_thickness = 1.f;
	vars.movement.autoduck = false;
	vars.movement.autoduckbefore = 0.05f;
	vars.movement.autoduckafter = 0.05f;
	vars.visuals.teamdmgx = 0.2;
	vars.visuals.teamdmgy = 0.5;
	vars.visuals.cooldownsay = false;
	vars.visuals.spotifysong = false;
	vars.visuals.addcounter = false;
	vars.legitbot.triggerbot.enabled = false;
	vars.legitbot.triggerbot.delay = 0.f;
	vars.legitbot.triggerbot.hitboxes.fill(false);
	vars.legitbot.triggerbot.mindmg = 1;
	vars.legitbot.triggerbot.onkey = false;
	vars.legitbot.triggerbot.trigkey.key = 0;
	vars.legitbot.triggerbot.trigkey.state = false;
	vars.legitbot.triggerbot.trigkey.type = 0;
	vars.legitbot.triggerbot.checkflash = false;
	vars.legitbot.triggerbot.checksmoke = false;
	vars.legitbot.triggerbot.scopedonly = false;
	vars.legitbot.triggerbot.teammates = false;
	vars.movement.ladderglide = false;
	vars.movement.ladderglidekey.key = 0;
	vars.movement.ladderglidekey.state = false;
	vars.movement.ladderglidekey.type = 0;
	vars.misc.chicken.chickenbox = false;
	vars.misc.chicken.enable = false;
	vars.misc.chicken.fish = false;
	vars.misc.chicken.fishbox = false;
	vars.misc.chicken.fishcolor = color_t(255, 255, 255, 255);
	vars.misc.chicken.owner = false;
	vars.misc.chicken.revealkiller = false;
	vars.misc.chicken.saykiller = false;
	vars.misc.chicken.sidelist = false;
	vars.legitbot.triggerbot.shootatbacktrack = true;

	for (int i = 0; i < 32; i++)
		vars.visuals.cooldownsaytext[i] = 0;

	for (auto& cham : vars.chams)
	{
		cham.materials.clear();
		cham.materials.push_back(CGlobalVariables::Chams::Material());
		cham.materials.push_back(CGlobalVariables::Chams::Material());
	}

	for (auto& item : g_config.get_items())
		item = item_setting();

	vars.movement.checkpoint.enabled = false;
	vars.movement.checkpoint.teleportkey.key = 0;
	vars.movement.checkpoint.teleportkey.type = 0;
	vars.movement.checkpoint.teleportkey.state = false;
	vars.movement.checkpoint.checkpointkey.key = 0;
	vars.movement.checkpoint.checkpointkey.type = 0;
	vars.movement.checkpoint.checkpointkey.state = false;
	vars.movement.checkpoint.nextkey.key = 0;
	vars.movement.checkpoint.nextkey.type = 0;
	vars.movement.checkpoint.nextkey.state = false;
	vars.movement.checkpoint.prevkey.key = 0;
	vars.movement.checkpoint.prevkey.type = 0;
	vars.movement.checkpoint.prevkey.state = false;
	vars.movement.checkpoint.undokey.key = 0;
	vars.movement.checkpoint.undokey.type = 0;
	vars.movement.checkpoint.undokey.state = false;
	vars.movement.checkpoint.color = color_t(0, 255, 128, 255);
	vars.movement.checkpoint.teleportx = 0.9f;
	vars.movement.checkpoint.teleporty = 0.7f;
	vars.visuals.playermodelct = 0;
	vars.visuals.playermodelt = 0;
	vars.movement.autopixelsurf = false;
	vars.movement.autopixelsurfkey.key = 0;
	vars.movement.autopixelsurfkey.type = 0;
	vars.movement.autopixelsurfkey.state = false;
	vars.movement.autopixelsurf_ticks = 1;
	vars.movement.autopixelsurf_align.key = 0;
	vars.movement.autopixelsurf_align.type = 0;
	vars.movement.autopixelsurf_align.state = false;
	vars.movement.autopixelsurf_freestand = false;
	vars.movement.autopixelsurf_alignind = false;
	vars.movement.autopixelsurf_indcol1 = color_t(60, 255, 60, 255);
	vars.visuals.sunset_enabled = false;
	vars.visuals.rot_x = 0.f;
	vars.visuals.rot_y = 0.f;
	vars.visuals.rot_z = 0.f;
	vars.visuals.fog.enabled = false;
	vars.visuals.fog.density = 0.5f;
	vars.visuals.fog.color = color_t(0, 128, 128, 255);
	vars.visuals.fog.end = 1450;
	vars.visuals.fog.start = 0;

	vars.menu.menu_key = VK_INSERT;
	vars.misc.unlockinventoryaccess = false;
	vars.menu.window_opacity = 90;

	vars.visuals.editparticles = false;
	vars.visuals.smoke_alpha = 1.f;
	vars.visuals.molotov_particle_color = color_t(255, 255, 255, 255);
	vars.visuals.blood_particle_color = color_t(255, 255, 255, 255);
	vars.visuals.blood_particle_color = color_t(255, 255, 255, 255);
	vars.movement.showveltype = 0;
	vars.movement.graph.xscreenpos = 0.85f;
	vars.movement.graph.gaincolors = false;

	vars.visuals.world.weapons.indtype[0] = false;
	vars.visuals.world.weapons.indtype[1] = true;
	vars.visuals.world.projectiles.indtype[0] = false;
	vars.visuals.world.projectiles.indtype[1] = true;
	vars.visuals.weaponicontype[0] = true;
	vars.visuals.weaponicontype[1] = false;
	
	vars.movement.showstamina = false;
	vars.movement.staminatakeoff = false;
	vars.movement.staminascrpos = 0.9f;
	vars.movement.staminacol = color_t(220, 220, 220, 255);
	vars.movement.staminafade = false;
	vars.movement.staminafadevel = 30.f;

	vars.movement.graph.types[0] = true;
	vars.movement.graph.types[1] = true;
	vars.movement.graph.staminacol = color_t(180, 180, 220, 255);
	vars.movement.graph.staminagaincolors = false;
	vars.movement.graph.staminatextcol = color_t(220, 220, 220, 255);
	vars.movement.graph.staminatofade = 30.f;

	vars.movement.indicators.active_col = color_t(0, 255, 128, 255);
	vars.movement.indicators.passive_col = color_t(255, 255, 255, 255);
	vars.movement.indicators.height = 0.8f;
	memset(vars.movement.indicators.on, 0, sizeof(vars.movement.indicators.on));
	vars.movement.indicators.show_active = true;
	vars.visuals.box_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.name_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.hp_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.skeleton_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.glow_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.weapon_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.ammo_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.dlights_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.flags_color_occluded = color_t(255, 255, 255, 255);
	vars.visuals.out_of_fov_visiblecolor = color_t(255, 255, 255, 255);

	


	vars.menu.loaded_luas.clear();
	
	//g_config.get_items().clear();
	//g_config.get_items().push_back(item_setting{});
}

void CConfig::Setup()
{
	ResetToDefault();
};

string CConfig::GetModuleFilePath(HMODULE hModule)
{
	string ModuleName = crypt_str("");
	char szFileName[MAX_PATH] = { 0 };

	if (GetModuleFileNameA(hModule, szFileName, MAX_PATH))
		ModuleName = szFileName;

	return ModuleName;
}

string CConfig::GetModuleBaseDir(HMODULE hModule)
{
	string ModulePath = GetModuleFilePath(hModule);
	return ModulePath.substr(0, ModulePath.find_last_of(crypt_str("\\/")));
}

void CConfig::Save(string cfg_name)
{
	json_t configuration;

	auto& json = configuration[crypt_str("config")];
	json[crypt_str("name")] = cfg_name;
	SaveColor(menu_colors::main_color, crypt_str("main_color"), &json);
	SaveColor(menu_colors::background1, crypt_str("background1"), &json);
	SaveColor(menu_colors::background2, crypt_str("background2"), &json);

	auto& legitbot = json[crypt_str("legitbot")]; {
		legitbot[crypt_str("aimbot")] = vars.legitbot.aimbot;
		legitbot[crypt_str("autopistol")] = vars.legitbot.autopistol;
		legitbot[crypt_str("backtrack")] = vars.legitbot.backtrack;
		legitbot[crypt_str("btlength")] = vars.legitbot.btlength;
		legitbot[crypt_str("onkey")] = vars.legitbot.onkey;
		SaveBind(&vars.legitbot.key, crypt_str("key"), &legitbot);
		legitbot[crypt_str("teammates")] = vars.legitbot.teammates;
		legitbot[crypt_str("rcsstandalone")] = vars.legitbot.rcsstandalone;
		for (int i = 0; i < vars.legitbot.weapons.size(); i++)
		{
			auto& weapon = legitbot[crypt_str("weapon")][i];
			weapon[crypt_str("aimbacktrack")] = vars.legitbot.weapons[i].aimbacktrack;
			weapon[crypt_str("aimlock")] = vars.legitbot.weapons[i].aimlock;
			weapon[crypt_str("silent")] = vars.legitbot.weapons[i].silent;
			weapon[crypt_str("enabled")] = vars.legitbot.weapons[i].enabled;
			weapon[crypt_str("fov")] = vars.legitbot.weapons[i].fov;
			weapon[crypt_str("hitbox0")] = vars.legitbot.weapons[i].hitboxes[0];
			weapon[crypt_str("hitbox1")] = vars.legitbot.weapons[i].hitboxes[1];
			weapon[crypt_str("hitbox2")] = vars.legitbot.weapons[i].hitboxes[2];
			weapon[crypt_str("hitbox3")] = vars.legitbot.weapons[i].hitboxes[3];
			weapon[crypt_str("hitbox4")] = vars.legitbot.weapons[i].hitboxes[4];
			weapon[crypt_str("ignoreflash")] = vars.legitbot.weapons[i].ignoreflash;
			weapon[crypt_str("ignoresmoke")] = vars.legitbot.weapons[i].ignoresmoke;
			weapon[crypt_str("rcsx")] = vars.legitbot.weapons[i].rcsx;
			weapon[crypt_str("rcsy")] = vars.legitbot.weapons[i].rcsy;
			weapon[crypt_str("scopedonly")] = vars.legitbot.weapons[i].scopedonly;
			weapon[crypt_str("smooth")] = vars.legitbot.weapons[i].smooth;
		}

		auto& triggerbot = legitbot[crypt_str("triggerbot")]; {
			triggerbot[crypt_str("enabled")] = vars.legitbot.triggerbot.enabled;
			triggerbot[crypt_str("onkey")] = vars.legitbot.triggerbot.onkey;
			SaveBind(&vars.legitbot.triggerbot.trigkey, crypt_str("trigkey"), &triggerbot);
			triggerbot[crypt_str("delay")] = vars.legitbot.triggerbot.delay;
			triggerbot[crypt_str("mindmg")] = vars.legitbot.triggerbot.mindmg;
			for (int i = 0; i < 5; i++)
			{
				auto& curhitbox = triggerbot[crypt_str("hitboxes")][i];
				curhitbox[crypt_str("box")] = vars.legitbot.triggerbot.hitboxes[i];
			}
			triggerbot[crypt_str("checkflash")] = vars.legitbot.triggerbot.checkflash;
			triggerbot[crypt_str("checksmoke")] = vars.legitbot.triggerbot.checksmoke;
			triggerbot[crypt_str("scopedonly")] = vars.legitbot.triggerbot.scopedonly;
			triggerbot[crypt_str("teammates")] = vars.legitbot.triggerbot.teammates;
			triggerbot[crypt_str("shootatbacktrack")] = vars.legitbot.triggerbot.shootatbacktrack;
		}
	}

	auto& ragebot = json[crypt_str("ragebot")]; {
		ragebot[crypt_str("enable")] = vars.ragebot.enable;
		ragebot[crypt_str("rage_onkey")] = vars.ragebot.onkey;
		SaveBind(&vars.ragebot.key, crypt_str("rage_key"), &ragebot);
		ragebot[crypt_str("auto_shoot")] = vars.ragebot.autoshoot;
		ragebot[crypt_str("silent")] = vars.ragebot.silent;
		ragebot[crypt_str("fov")] = vars.ragebot.fov;
		ragebot[crypt_str("auto_scope")] = vars.ragebot.autoscope;
		ragebot[crypt_str("zeusbot")] = vars.ragebot.zeusbot;
		ragebot[crypt_str("zeuschance")] = vars.ragebot.zeuschance;
		ragebot[crypt_str("resolver")] = vars.ragebot.resolver;
		//ragebot[crypt_str("shot_record")] = vars.ragebot.shotrecord;
		//ragebot[crypt_str("extended_backtrack")] = vars.ragebot.extended_backtrack;
		ragebot[crypt_str("dt_teleport")] = vars.ragebot.dt_teleport;
		ragebot[crypt_str("antiexploit")] = vars.ragebot.antiexploit;
		ragebot[crypt_str("hideshot")] = vars.ragebot.hideshot;

		SaveBind(&vars.ragebot.override_dmg, crypt_str("override_dmg"), &ragebot);
		SaveBind(&vars.ragebot.double_tap, crypt_str("double_tap"), &ragebot);
		SaveBind(&vars.ragebot.hideShots, crypt_str("hideShots"), &ragebot);
		SaveBind(&vars.ragebot.force_safepoint, crypt_str("force_safepoint"), &ragebot);

		SaveBind(&vars.ragebot.baim, crypt_str("baim"), &ragebot);
		ragebot[crypt_str("baim_type")] = vars.ragebot.baim_type;

		//SaveColor(vars.ragebot.shot_clr, crypt_str("shot_clr"), &ragebot);

		auto& weapon_cfg = ragebot[crypt_str("weapon_cfg")];
		for (int i = 0; i < 7; i++) {
			string category = crypt_str("weapon_") + std::to_string(i);
			weapon_cfg[category][crypt_str("enable")] = vars.ragebot.weapon[i].enable;
			weapon_cfg[category][crypt_str("mindamage")] = vars.ragebot.weapon[i].mindamage;
			weapon_cfg[category][crypt_str("mindamage_override")] = vars.ragebot.weapon[i].mindamage_override;
			weapon_cfg[category][crypt_str("hitchance")] = vars.ragebot.weapon[i].hitchance;
			weapon_cfg[category][crypt_str("doubletap_hc")] = vars.ragebot.weapon[i].doubletap_hc;
			weapon_cfg[category][crypt_str("multipoint")] = vars.ragebot.weapon[i].multipoint;
			//weapon_cfg[category][crypt_str("quickstop")] = vars.ragebot.weapon[i].quickstop;
			weapon_cfg[category][crypt_str("autostopMode")] = vars.ragebot.weapon[i].autostopMode;
			//weapon_cfg[category][crypt_str("quickstop_options")] = vars.ragebot.weapon[i].quickstop_options;
			//weapon_cfg[category][crypt_str("hitboxes")] = vars.ragebot.weapon[i].hitscan;

			weapon_cfg[category][crypt_str("hitbox0")] = vars.ragebot.weapon[i].hitboxes[0];
			weapon_cfg[category][crypt_str("hitbox1")] = vars.ragebot.weapon[i].hitboxes[1];
			weapon_cfg[category][crypt_str("hitbox2")] = vars.ragebot.weapon[i].hitboxes[2];
			weapon_cfg[category][crypt_str("hitbox3")] = vars.ragebot.weapon[i].hitboxes[3];
			weapon_cfg[category][crypt_str("hitbox4")] = vars.ragebot.weapon[i].hitboxes[4];
			weapon_cfg[category][crypt_str("hitbox5")] = vars.ragebot.weapon[i].hitboxes[5];
			weapon_cfg[category][crypt_str("hitbox6")] = vars.ragebot.weapon[i].hitboxes[6];
			weapon_cfg[category][crypt_str("hitbox7")] = vars.ragebot.weapon[i].hitboxes[7];
			weapon_cfg[category][crypt_str("hitbox8")] = vars.ragebot.weapon[i].hitboxes[8];
			weapon_cfg[category][crypt_str("hitbox9")] = vars.ragebot.weapon[i].hitboxes[9];
			weapon_cfg[category][crypt_str("hitbox10")] = vars.ragebot.weapon[i].hitboxes[10];

			weapon_cfg[category][crypt_str("static_scale")] = vars.ragebot.weapon[i].static_scale;
			weapon_cfg[category][crypt_str("scale_body")] = vars.ragebot.weapon[i].scale_body;
			weapon_cfg[category][crypt_str("scale_head")] = vars.ragebot.weapon[i].scale_head;
			weapon_cfg[category][crypt_str("prefer_safepoint")] = vars.ragebot.weapon[i].prefer_safepoint;
			weapon_cfg[category][crypt_str("max_misses")] = vars.ragebot.weapon[i].max_misses;
		}
	}

	auto& antiaim = json[crypt_str("antiaim")]; {
		antiaim[crypt_str("enable")] = vars.antiaim.enable;
		antiaim[crypt_str("shotDesync")] = vars.antiaim.shotDesync;
		antiaim[crypt_str("aa_on_use")] = vars.antiaim.aa_on_use;
		antiaim[crypt_str("pitch")] = vars.antiaim.pitch;
		antiaim[crypt_str("jitter_angle")] = vars.antiaim.jitter_angle;
		antiaim[crypt_str("zlean")] = vars.antiaim.zlean;
		antiaim[crypt_str("zleanenable")] = vars.antiaim.zleanenable;
		antiaim[crypt_str("at_target")] = vars.antiaim.attarget;
		antiaim[crypt_str("attarget_off_when_offsreen")] = vars.antiaim.attarget_off_when_offsreen;

		SaveBind(&vars.antiaim.fakeduck, crypt_str("fakeduck"), &antiaim);
		SaveBind(&vars.antiaim.slowwalk, crypt_str("slowwalk"), &antiaim);
		antiaim[crypt_str("slowwalkspeed")] = vars.antiaim.slowwalkspeed;
		SaveBind(&vars.antiaim.inverter, crypt_str("inverter"), &antiaim);

		antiaim[crypt_str("static_delta")] = vars.antiaim.static_delta;
		antiaim[crypt_str("desync_amount")] = vars.antiaim.desync_amount;
		antiaim[crypt_str("base_yaw")] = vars.antiaim.base_yaw;
		antiaim[crypt_str("avoid_overlap")] = vars.antiaim.avoid_overlap;
		antiaim[crypt_str("manual_antiaim")] = vars.antiaim.manual_antiaim;
		antiaim[crypt_str("ignore_attarget")] = vars.antiaim.ignore_attarget;
		antiaim[crypt_str("simtimeStopper")] = vars.antiaim.simtimeStopper;

		SaveBind(&vars.antiaim.manual_left, crypt_str("manual_left"), &antiaim);
		SaveBind(&vars.antiaim.manual_right, crypt_str("manual_right"), &antiaim);
		SaveBind(&vars.antiaim.manual_back, crypt_str("manual_back"), &antiaim);
		SaveBind(&vars.antiaim.manual_forward, crypt_str("manual_forward"), &antiaim);

		auto& fakelag = antiaim[crypt_str("fakelag")]; {
			fakelag[crypt_str("type")] = vars.antiaim.fakelag;
			fakelag[crypt_str("when_standing")] = vars.antiaim.fakelag_when_standing;
			fakelag[crypt_str("when_exploits")] = vars.antiaim.fakelag_when_exploits;
			fakelag[crypt_str("factor")] = vars.antiaim.fakelagfactor;
			fakelag[crypt_str("switch_ticks")] = vars.antiaim.fakelagvariance;
			fakelag[crypt_str("breakLC")] = vars.antiaim.fakelagvariance;
		}

		antiaim[crypt_str("extend_lby")] = vars.antiaim.extendlby;
		SaveBind(&vars.antiaim.fakehead, crypt_str("fakehead"), &antiaim);
		antiaim[crypt_str("fakehead_mode")] = vars.antiaim.fakehead_mode;
	}

	

	auto& players = json[crypt_str("players")]; {
		auto& esp = players[crypt_str("esp")]; {
			esp[crypt_str("enable")] = vars.visuals.enable;
			esp[crypt_str("dormant")] = vars.visuals.dormant;
			esp[crypt_str("zeus_warning")] = vars.visuals.zeus_warning;

			esp[crypt_str("box")] = vars.visuals.box; {
				esp[crypt_str("box_color")][crypt_str("red")] = vars.visuals.box_color.get_red();
				esp[crypt_str("box_color")][crypt_str("green")] = vars.visuals.box_color.get_green();
				esp[crypt_str("box_color")][crypt_str("blue")] = vars.visuals.box_color.get_blue();
				esp[crypt_str("box_color")][crypt_str("alpha")] = vars.visuals.box_color.get_alpha();
			}
			SaveColor(vars.visuals.box_color_occluded, crypt_str("box_color_occluded"), &esp);

			esp[crypt_str("skeleton")] = vars.visuals.skeleton; {
				esp[crypt_str("skeleton_color")][crypt_str("red")] = vars.visuals.skeleton_color.get_red();
				esp[crypt_str("skeleton_color")][crypt_str("green")] = vars.visuals.skeleton_color.get_green();
				esp[crypt_str("skeleton_color")][crypt_str("blue")] = vars.visuals.skeleton_color.get_blue();
				esp[crypt_str("skeleton_color")][crypt_str("alpha")] = vars.visuals.skeleton_color.get_alpha();
			}
			SaveColor(vars.visuals.skeleton_color_occluded, crypt_str("skeleton_color_occluded"), &esp);

			esp[crypt_str("healthbar")][crypt_str("enable")] = vars.visuals.healthbar;
			esp[crypt_str("healthbar")][crypt_str("override_hp")] = vars.visuals.override_hp; {
				esp[crypt_str("healthbar")][crypt_str("hp_color")][crypt_str("red")] = vars.visuals.hp_color.get_red();
				esp[crypt_str("healthbar")][crypt_str("hp_color")][crypt_str("green")] = vars.visuals.hp_color.get_green();
				esp[crypt_str("healthbar")][crypt_str("hp_color")][crypt_str("blue")] = vars.visuals.hp_color.get_blue();
				esp[crypt_str("healthbar")][crypt_str("hp_color")][crypt_str("alpha")] = vars.visuals.hp_color.get_alpha();
			}
			SaveColor(vars.visuals.hp_color_occluded, crypt_str("hp_color_occluded"), &esp);

			esp[crypt_str("name")] = vars.visuals.name; {
				esp[crypt_str("name_color")][crypt_str("red")] = vars.visuals.name_color.get_red();
				esp[crypt_str("name_color")][crypt_str("green")] = vars.visuals.name_color.get_green();
				esp[crypt_str("name_color")][crypt_str("blue")] = vars.visuals.name_color.get_blue();
				esp[crypt_str("name_color")][crypt_str("alpha")] = vars.visuals.name_color.get_alpha();
			}
			SaveColor(vars.visuals.name_color_occluded, crypt_str("name_color_occluded"), &esp);

			esp[crypt_str("weapon")] = vars.visuals.weapon; {
				esp[crypt_str("weapon_color")][crypt_str("red")] = vars.visuals.weapon_color.get_red();
				esp[crypt_str("weapon_color")][crypt_str("green")] = vars.visuals.weapon_color.get_green();
				esp[crypt_str("weapon_color")][crypt_str("blue")] = vars.visuals.weapon_color.get_blue();
				esp[crypt_str("weapon_color")][crypt_str("alpha")] = vars.visuals.weapon_color.get_alpha();
			}
			SaveColor(vars.visuals.weapon_color_occluded, crypt_str("weapon_color_occluded"), &esp);

			esp[crypt_str("weaponicontype0")] = vars.visuals.weaponicontype[0];
			esp[crypt_str("weaponicontype1")] = vars.visuals.weaponicontype[1];

			esp[crypt_str("ammo")] = vars.visuals.ammo; {
				esp[crypt_str("ammo_color")][crypt_str("red")] = vars.visuals.ammo_color.get_red();
				esp[crypt_str("ammo_color")][crypt_str("green")] = vars.visuals.ammo_color.get_green();
				esp[crypt_str("ammo_color")][crypt_str("blue")] = vars.visuals.ammo_color.get_blue();
				esp[crypt_str("ammo_color")][crypt_str("alpha")] = vars.visuals.ammo_color.get_alpha();
			}
			SaveColor(vars.visuals.ammo_color_occluded, crypt_str("ammo_color_occluded"), &esp);

			esp[crypt_str("flags")] = vars.visuals.flags; {
				esp[crypt_str("flags_color")][crypt_str("red")] = vars.visuals.flags_color.get_red();
				esp[crypt_str("flags_color")][crypt_str("green")] = vars.visuals.flags_color.get_green();
				esp[crypt_str("flags_color")][crypt_str("blue")] = vars.visuals.flags_color.get_blue();
				esp[crypt_str("flags_color")][crypt_str("alpha")] = vars.visuals.flags_color.get_alpha();
			}
			SaveColor(vars.visuals.flags_color_occluded, crypt_str("flags_color_occluded"), &esp);

			esp[crypt_str("show_multipoint")] = vars.visuals.shot_multipoint;
			esp[crypt_str("out_of_fov")][crypt_str("enable")] = vars.visuals.out_of_fov; {
				esp[crypt_str("out_of_fov")][crypt_str("distance")] = vars.visuals.out_of_fov_distance;
				esp[crypt_str("out_of_fov")][crypt_str("size")] = vars.visuals.out_of_fov_size;
				esp[crypt_str("out_of_fov")][crypt_str("color")][crypt_str("red")] = vars.visuals.out_of_fov_color.get_red();
				esp[crypt_str("out_of_fov")][crypt_str("color")][crypt_str("green")] = vars.visuals.out_of_fov_color.get_green();
				esp[crypt_str("out_of_fov")][crypt_str("color")][crypt_str("blue")] = vars.visuals.out_of_fov_color.get_blue();
				esp[crypt_str("out_of_fov")][crypt_str("color")][crypt_str("alpha")] = vars.visuals.out_of_fov_color.get_alpha();
				esp[crypt_str("out_of_fov")][crypt_str("drawvisible")] = vars.visuals.out_of_fov_drawvisible;
				SaveColor(vars.visuals.out_of_fov_visiblecolor, crypt_str("visiblecolor"), &esp[crypt_str("out_of_fov")]);
			}

			esp[crypt_str("dlights")] = vars.visuals.dlights;
			SaveColor(vars.visuals.dlights_color, crypt_str("dlights_color"), &esp);
			SaveColor(vars.visuals.dlights_color_occluded, crypt_str("dlights_color_occluded"), &esp);
		}
		auto& models = players[crypt_str("models")]; {  // not in use
			auto& chams_cfg = models[crypt_str("chams")]; {
				chams_cfg[crypt_str("ragdoll_force")] = vars.visuals.ragdoll_force;
				chams_cfg[crypt_str("enemy")][crypt_str("enable")] = vars.visuals.chams; {
					chams_cfg[crypt_str("enemy")][crypt_str("visible_color")][crypt_str("red")] = vars.visuals.chamscolor.get_red();
					chams_cfg[crypt_str("enemy")][crypt_str("visible_color")][crypt_str("green")] = vars.visuals.chamscolor.get_green();
					chams_cfg[crypt_str("enemy")][crypt_str("visible_color")][crypt_str("blue")] = vars.visuals.chamscolor.get_blue();
					chams_cfg[crypt_str("enemy")][crypt_str("visible_color")][crypt_str("alpha")] = vars.visuals.chamscolor.get_alpha();

					chams_cfg[crypt_str("enemy")][crypt_str("through_walls")] = vars.visuals.chamsxqz;
					chams_cfg[crypt_str("enemy")][crypt_str("chams_ragdoll")] = vars.visuals.chams_ragdoll;
					chams_cfg[crypt_str("enemy")][crypt_str("through_walls_color")][crypt_str("red")] = vars.visuals.chamscolor_xqz.get_red();
					chams_cfg[crypt_str("enemy")][crypt_str("through_walls_color")][crypt_str("green")] = vars.visuals.chamscolor_xqz.get_green();
					chams_cfg[crypt_str("enemy")][crypt_str("through_walls_color")][crypt_str("blue")] = vars.visuals.chamscolor_xqz.get_blue();
					chams_cfg[crypt_str("enemy")][crypt_str("through_walls_color")][crypt_str("alpha")] = vars.visuals.chamscolor_xqz.get_alpha();

					chams_cfg[crypt_str("enemy")][crypt_str("overlay_color")][crypt_str("red")] = vars.visuals.glow_col.get_red();
					chams_cfg[crypt_str("enemy")][crypt_str("overlay_color")][crypt_str("green")] = vars.visuals.glow_col.get_green();
					chams_cfg[crypt_str("enemy")][crypt_str("overlay_color")][crypt_str("blue")] = vars.visuals.glow_col.get_blue();
					chams_cfg[crypt_str("enemy")][crypt_str("overlay_color")][crypt_str("alpha")] = vars.visuals.glow_col.get_alpha();

					chams_cfg[crypt_str("enemy")][crypt_str("xqz_overlay_color")][crypt_str("red")] = vars.visuals.glow_col_xqz.get_red();
					chams_cfg[crypt_str("enemy")][crypt_str("xqz_overlay_color")][crypt_str("green")] = vars.visuals.glow_col_xqz.get_green();
					chams_cfg[crypt_str("enemy")][crypt_str("xqz_overlay_color")][crypt_str("blue")] = vars.visuals.glow_col_xqz.get_blue();
					chams_cfg[crypt_str("enemy")][crypt_str("xqz_overlay_color")][crypt_str("alpha")] = vars.visuals.glow_col_xqz.get_alpha();

					chams_cfg[crypt_str("enemy")][crypt_str("material")] = vars.visuals.chamstype;
					chams_cfg[crypt_str("enemy")][crypt_str("phong_exponent")] = vars.visuals.phong_exponent;
					chams_cfg[crypt_str("enemy")][crypt_str("phong_boost")] = vars.visuals.phong_boost;
					chams_cfg[crypt_str("enemy")][crypt_str("rim")] = vars.visuals.rim;
					chams_cfg[crypt_str("enemy")][crypt_str("brightness")] = vars.visuals.chams_brightness;
					chams_cfg[crypt_str("enemy")][crypt_str("pearlescent")] = vars.visuals.pearlescent;
					chams_cfg[crypt_str("enemy")][crypt_str("overlay")] = vars.visuals.overlay;
					chams_cfg[crypt_str("enemy")][crypt_str("overlay_xqz")] = vars.visuals.overlay_xqz;

					chams_cfg[crypt_str("enemy")][crypt_str("metallic_color")][crypt_str("red")] = vars.visuals.metallic_clr.get_red();
					chams_cfg[crypt_str("enemy")][crypt_str("metallic_color")][crypt_str("green")] = vars.visuals.metallic_clr.get_green();
					chams_cfg[crypt_str("enemy")][crypt_str("metallic_color")][crypt_str("blue")] = vars.visuals.metallic_clr.get_blue();
					chams_cfg[crypt_str("enemy")][crypt_str("metallic_color")][crypt_str("alpha")] = vars.visuals.metallic_clr.get_alpha();

					chams_cfg[crypt_str("enemy")][crypt_str("phong_color")][crypt_str("red")] = vars.visuals.metallic_clr2.get_red();
					chams_cfg[crypt_str("enemy")][crypt_str("phong_color")][crypt_str("green")] = vars.visuals.metallic_clr2.get_green();
					chams_cfg[crypt_str("enemy")][crypt_str("phong_color")][crypt_str("blue")] = vars.visuals.metallic_clr2.get_blue();
					chams_cfg[crypt_str("enemy")][crypt_str("phong_color")][crypt_str("alpha")] = vars.visuals.metallic_clr2.get_alpha();
				}

				chams_cfg[crypt_str("local")][crypt_str("enable")] = vars.visuals.localchams; {
					chams_cfg[crypt_str("local")][crypt_str("material")] = vars.visuals.localchamstype;
					chams_cfg[crypt_str("local")][crypt_str("blend_on_scope")] = vars.visuals.blend_on_scope;
					chams_cfg[crypt_str("local")][crypt_str("blend_value")] = vars.visuals.blend_value;

					chams_cfg[crypt_str("local")][crypt_str("overlay_color")][crypt_str("red")] = vars.visuals.local_glow_color.get_red();
					chams_cfg[crypt_str("local")][crypt_str("overlay_color")][crypt_str("green")] = vars.visuals.local_glow_color.get_green();
					chams_cfg[crypt_str("local")][crypt_str("overlay_color")][crypt_str("blue")] = vars.visuals.local_glow_color.get_blue();
					chams_cfg[crypt_str("local")][crypt_str("overlay_color")][crypt_str("alpha")] = vars.visuals.local_glow_color.get_alpha();

					chams_cfg[crypt_str("local")][crypt_str("visible_color")][crypt_str("red")] = vars.visuals.localchams_color.get_red();
					chams_cfg[crypt_str("local")][crypt_str("visible_color")][crypt_str("green")] = vars.visuals.localchams_color.get_green();
					chams_cfg[crypt_str("local")][crypt_str("visible_color")][crypt_str("blue")] = vars.visuals.localchams_color.get_blue();
					chams_cfg[crypt_str("local")][crypt_str("visible_color")][crypt_str("alpha")] = vars.visuals.localchams_color.get_alpha();

					chams_cfg[crypt_str("local")][crypt_str("phong_exponent")] = vars.visuals.local_chams.phong_exponent;
					chams_cfg[crypt_str("local")][crypt_str("phong_boost")] = vars.visuals.local_chams.phong_boost;
					chams_cfg[crypt_str("local")][crypt_str("rim")] = vars.visuals.local_chams.rim;
					chams_cfg[crypt_str("local")][crypt_str("brightness")] = vars.visuals.local_chams_brightness;
					chams_cfg[crypt_str("local")][crypt_str("pearlescent")] = vars.visuals.local_chams.pearlescent;
					chams_cfg[crypt_str("local")][crypt_str("overlay")] = vars.visuals.local_chams.overlay;

					chams_cfg[crypt_str("local")][crypt_str("metallic_color")][crypt_str("red")] = vars.visuals.local_chams.metallic_clr.get_red();
					chams_cfg[crypt_str("local")][crypt_str("metallic_color")][crypt_str("green")] = vars.visuals.local_chams.metallic_clr.get_green();
					chams_cfg[crypt_str("local")][crypt_str("metallic_color")][crypt_str("blue")] = vars.visuals.local_chams.metallic_clr.get_blue();
					chams_cfg[crypt_str("local")][crypt_str("metallic_color")][crypt_str("alpha")] = vars.visuals.local_chams.metallic_clr.get_alpha();

					chams_cfg[crypt_str("local")][crypt_str("phong_color")][crypt_str("red")] = vars.visuals.local_chams.metallic_clr2.get_red();
					chams_cfg[crypt_str("local")][crypt_str("phong_color")][crypt_str("green")] = vars.visuals.local_chams.metallic_clr2.get_green();
					chams_cfg[crypt_str("local")][crypt_str("phong_color")][crypt_str("blue")] = vars.visuals.local_chams.metallic_clr2.get_blue();
					chams_cfg[crypt_str("local")][crypt_str("phong_color")][crypt_str("alpha")] = vars.visuals.local_chams.metallic_clr2.get_alpha();
				}

				chams_cfg[crypt_str("misc_chams_2")][crypt_str("interpolated")] = vars.visuals.interpolated_bt;
				chams_cfg[crypt_str("misc_chams_3")][crypt_str("interpolated")] = vars.visuals.interpolated_dsy;
				for (int i = 0; i < 5; i++) {
					string category = crypt_str("misc_chams_") + std::to_string(i);
					chams_cfg[category][crypt_str("enable")] = vars.visuals.misc_chams[i].enable;
					chams_cfg[category][crypt_str("material")] = vars.visuals.misc_chams[i].material;
					chams_cfg[category][crypt_str("brightness")] = vars.visuals.misc_chams[i].chams_brightness;
					chams_cfg[category][crypt_str("pearlescent")] = vars.visuals.misc_chams[i].pearlescent;
					chams_cfg[category][crypt_str("phong_exponent")] = vars.visuals.misc_chams[i].phong_exponent;
					chams_cfg[category][crypt_str("phong_boost")] = vars.visuals.misc_chams[i].phong_boost;
					chams_cfg[category][crypt_str("rim")] = vars.visuals.misc_chams[i].rim;
					chams_cfg[category][crypt_str("overlay")] = vars.visuals.misc_chams[i].overlay;

					chams_cfg[category][crypt_str("model_color")][crypt_str("red")] = vars.visuals.misc_chams[i].clr.get_red();
					chams_cfg[category][crypt_str("model_color")][crypt_str("green")] = vars.visuals.misc_chams[i].clr.get_green();
					chams_cfg[category][crypt_str("model_color")][crypt_str("blue")] = vars.visuals.misc_chams[i].clr.get_blue();
					chams_cfg[category][crypt_str("model_color")][crypt_str("alpha")] = vars.visuals.misc_chams[i].clr.get_alpha();

					chams_cfg[category][crypt_str("overlay_color")][crypt_str("red")] = vars.visuals.misc_chams[i].glow_clr.get_red();
					chams_cfg[category][crypt_str("overlay_color")][crypt_str("green")] = vars.visuals.misc_chams[i].glow_clr.get_green();
					chams_cfg[category][crypt_str("overlay_color")][crypt_str("blue")] = vars.visuals.misc_chams[i].glow_clr.get_blue();
					chams_cfg[category][crypt_str("overlay_color")][crypt_str("alpha")] = vars.visuals.misc_chams[i].glow_clr.get_alpha();

					chams_cfg[category][crypt_str("metallic_color")][crypt_str("red")] = vars.visuals.misc_chams[i].metallic_clr.get_red();
					chams_cfg[category][crypt_str("metallic_color")][crypt_str("green")] = vars.visuals.misc_chams[i].metallic_clr.get_green();
					chams_cfg[category][crypt_str("metallic_color")][crypt_str("blue")] = vars.visuals.misc_chams[i].metallic_clr.get_blue();
					chams_cfg[category][crypt_str("metallic_color")][crypt_str("alpha")] = vars.visuals.misc_chams[i].metallic_clr.get_alpha();

					chams_cfg[category][crypt_str("phong_color")][crypt_str("red")] = vars.visuals.misc_chams[i].metallic_clr2.get_red();
					chams_cfg[category][crypt_str("phong_color")][crypt_str("green")] = vars.visuals.misc_chams[i].metallic_clr2.get_green();
					chams_cfg[category][crypt_str("phong_color")][crypt_str("blue")] = vars.visuals.misc_chams[i].metallic_clr2.get_blue();
					chams_cfg[category][crypt_str("phong_color")][crypt_str("alpha")] = vars.visuals.misc_chams[i].metallic_clr2.get_alpha();
				}
			}
			auto& glow = models[crypt_str("glow")]; {
				glow[crypt_str("style")] = vars.visuals.glowtype;

				glow[crypt_str("enemy")][crypt_str("enable")] = vars.visuals.glow;
				glow[crypt_str("enemy")][crypt_str("color")][crypt_str("red")] = vars.visuals.glow_color.get_red();
				glow[crypt_str("enemy")][crypt_str("color")][crypt_str("green")] = vars.visuals.glow_color.get_green();
				glow[crypt_str("enemy")][crypt_str("color")][crypt_str("blue")] = vars.visuals.glow_color.get_blue();
				glow[crypt_str("enemy")][crypt_str("color")][crypt_str("alpha")] = vars.visuals.glow_color.get_alpha();

				SaveColor(vars.visuals.glow_color_occluded, crypt_str("glow_color_occluded"), &glow);

				glow[crypt_str("local")][crypt_str("enable")] = vars.visuals.local_glow;
				glow[crypt_str("local")][crypt_str("color")][crypt_str("red")] = vars.visuals.local_glow_clr.get_red();
				glow[crypt_str("local")][crypt_str("color")][crypt_str("green")] = vars.visuals.local_glow_clr.get_green();
				glow[crypt_str("local")][crypt_str("color")][crypt_str("blue")] = vars.visuals.local_glow_clr.get_blue();
				glow[crypt_str("local")][crypt_str("color")][crypt_str("alpha")] = vars.visuals.local_glow_clr.get_alpha();
			}
		}
		auto& misc = players[crypt_str("misc")]; {
			misc[crypt_str("preverse_killfeed")][crypt_str("enable")] = vars.visuals.preverse_killfeed;

			misc[crypt_str("nadepred")][crypt_str("enable")] = vars.visuals.nadepred;
			SaveColor(vars.visuals.nadepred_color, crypt_str("color"), &misc[crypt_str("nadepred")]);

			misc[crypt_str("taser_range")][crypt_str("enable")] = vars.visuals.taser_range;
			SaveColor(vars.visuals.taser_range_color, crypt_str("color"), &misc[crypt_str("taser_range")]);

			misc[crypt_str("indicators")] = vars.visuals.indicators;
			misc[crypt_str("indicators_rage")] = vars.visuals.indicators_rage;
			misc[crypt_str("antiaim_arrows")] = vars.visuals.antiaim_arrows;
			SaveColor(vars.visuals.antiaim_arrows_color, crypt_str("antiaim_arrows_color"), &misc);

			misc[crypt_str("aspect_ratio")] = vars.visuals.aspect_ratio;
			misc[crypt_str("thirdperson")][crypt_str("distance")] = vars.visuals.thirdperson_dist;
			SaveBind(&vars.misc.thirdperson, crypt_str("bind"), &misc[crypt_str("thirdperson")]);

			SaveBind(&vars.misc.peek_assist, crypt_str("peek_assist"), &misc);

			SaveBind(&vars.misc.blockbot, crypt_str("blockbot"), &misc);

			misc[crypt_str("svpure")] = vars.misc.sv_purebypass;

			misc[crypt_str("watermark")] = vars.visuals.watermark;

			misc[crypt_str("speclist")] = vars.visuals.speclist;
			misc[crypt_str("speclistx")] = vars.menu.cspeclistx;
			misc[crypt_str("speclisty")] = vars.menu.cspeclisty;
			misc[crypt_str("keybindx")] = vars.menu.ckeybindx;
			misc[crypt_str("keybindy")] = vars.menu.ckeybindy;
			misc[crypt_str("infowindowx")] = vars.menu.crecorderx;
			misc[crypt_str("infowindowy")] = vars.menu.crecordery;
			misc[crypt_str("teleportx")] = vars.menu.cteleportx;
			misc[crypt_str("teleporty")] = vars.menu.cteleporty;

			misc[crypt_str("teamdmglist")] = vars.visuals.teamdmglist;
			misc[crypt_str("teamdmgx")] = vars.menu.cteamdmgx;
			misc[crypt_str("teamdmgy")] = vars.menu.cteamdmgy;
			misc[crypt_str("cooldownsay")] = vars.visuals.cooldownsay;
			misc[crypt_str("cooldownsaytext")] = vars.visuals.cooldownsaytext;
			misc[crypt_str("addcounter")] = vars.visuals.addcounter;

			misc[crypt_str("autoaccept")] = vars.misc.autoaccept;
			misc[crypt_str("autoacceptdelay")] = vars.misc.autoacceptdelay;
			misc[crypt_str("killsay")] = vars.misc.killsay;
			
			misc[crypt_str("eventlog")][crypt_str("enable")] = vars.visuals.eventlog;
			
			misc[crypt_str("eventlog")][crypt_str("color")][crypt_str("red")] = vars.visuals.eventlog_color.get_red();
			misc[crypt_str("eventlog")][crypt_str("color")][crypt_str("green")] = vars.visuals.eventlog_color.get_green();
			misc[crypt_str("eventlog")][crypt_str("color")][crypt_str("blue")] = vars.visuals.eventlog_color.get_blue();
			misc[crypt_str("eventlog")][crypt_str("color")][crypt_str("alpha")] = vars.visuals.eventlog_color.get_alpha();

			misc[crypt_str("eventlog")][crypt_str("print_votes")] = vars.visuals.print_votes;
			misc[crypt_str("eventlog")][crypt_str("print_votes_chat")] = vars.visuals.print_votes_chat;
			//visualize_damage
			misc[crypt_str("hitmarker")][crypt_str("enable")] = vars.visuals.hitmarker;
			misc[crypt_str("hitmarker")][crypt_str("visualize_damage")] = vars.visuals.visualize_damage;
			misc[crypt_str("hitmarker")][crypt_str("enable_sound")] = vars.visuals.hitmarker_sound;
			misc[crypt_str("hitmarker")][crypt_str("hitmarker_sound_type")] = vars.visuals.hitmarker_sound_type;
			misc[crypt_str("hitmarker")][crypt_str("hitmarker_name")] = vars.visuals.hitmarker_name;

			misc[crypt_str("hitmarker")][crypt_str("color")][crypt_str("red")] = vars.visuals.hitmarker_color.get_red();
			misc[crypt_str("hitmarker")][crypt_str("color")][crypt_str("green")] = vars.visuals.hitmarker_color.get_green();
			misc[crypt_str("hitmarker")][crypt_str("color")][crypt_str("blue")] = vars.visuals.hitmarker_color.get_blue();
			misc[crypt_str("hitmarker")][crypt_str("color")][crypt_str("alpha")] = vars.visuals.hitmarker_color.get_alpha();
			misc[crypt_str("spotifysong")] = vars.visuals.spotifysong;
			misc[crypt_str("sunset_enabled")] = vars.visuals.sunset_enabled;
			misc[crypt_str("rot_x")] = vars.visuals.rot_x;
			misc[crypt_str("rot_y")] = vars.visuals.rot_y;
			misc[crypt_str("rot_z")] = vars.visuals.rot_z;
			misc[crypt_str("editparticles")] = vars.visuals.editparticles;
			misc[crypt_str("smoke_alpha")] = vars.visuals.smoke_alpha;
			SaveColor(vars.visuals.molotov_particle_color, crypt_str("molotov_particle_color"), &misc);
			SaveColor(vars.visuals.blood_particle_color, crypt_str("blood_particle_color"), &misc);

			auto& fog = misc[crypt_str("fog")];
			{
				fog[crypt_str("enabled")] = vars.visuals.fog.enabled;
				fog[crypt_str("start")] = vars.visuals.fog.start;
				fog[crypt_str("end")] = vars.visuals.fog.end;
				fog[crypt_str("density")] = vars.visuals.fog.density;
				SaveColor(vars.visuals.fog.color, crypt_str("color"), &fog);
			}

			auto& motionblur = misc[crypt_str("motionblur")];
			{
				motionblur[crypt_str("enabled")] = vars.visuals.motionblur.enabled;
				motionblur[crypt_str("fwd_enabled")] = vars.visuals.motionblur.fwd_enabled;
				motionblur[crypt_str("falling_min")] = vars.visuals.motionblur.falling_min;
				motionblur[crypt_str("falling_max")] = vars.visuals.motionblur.falling_max;
				motionblur[crypt_str("falling_intensity")] = vars.visuals.motionblur.falling_intensity;
				motionblur[crypt_str("rotation_intensity")] = vars.visuals.motionblur.rotation_intensity;
				motionblur[crypt_str("strength")] = vars.visuals.motionblur.strength;
			}
		}
	}

	auto& world = json[crypt_str("world")]; {
		auto& weapons = world[crypt_str("weapons")]; {
			weapons[crypt_str("enable")] = vars.visuals.world.weapons.enabled;
			weapons[crypt_str("indtype0")] = vars.visuals.world.weapons.indtype[0];
			weapons[crypt_str("indtype1")] = vars.visuals.world.weapons.indtype[1];
			weapons[crypt_str("planted_bomb")] = vars.visuals.world.weapons.planted_bomb;
			weapons[crypt_str("planted_bombheight")] = vars.visuals.world.weapons.planted_bombheight;
			SaveColor(vars.visuals.world.weapons.color, crypt_str("color"), &weapons);
		}
		auto& projectiles = world[crypt_str("projectiles")]; {
			projectiles[crypt_str("filter0")] = vars.visuals.world.projectiles.filter[0];
			projectiles[crypt_str("filter1")] = vars.visuals.world.projectiles.filter[1];
			projectiles[crypt_str("filter2")] = vars.visuals.world.projectiles.filter[2];
			projectiles[crypt_str("indtype0")] = vars.visuals.world.projectiles.indtype[0];
			projectiles[crypt_str("indtype1")] = vars.visuals.world.projectiles.indtype[1];
			projectiles[crypt_str("enable")] = vars.visuals.world.projectiles.enable;
#ifdef _DEBUG
			projectiles[crypt_str("trajectories")] = vars.visuals.world.projectiles.trajectories;
			SaveColor(vars.visuals.world.projectiles.colors[0], crypt_str("trajectory_team"), &projectiles);
			SaveColor(vars.visuals.world.projectiles.colors[1], crypt_str("trajectory_enemy"), &projectiles);
			SaveColor(vars.visuals.world.projectiles.colors[2], crypt_str("trajectory_local"), &projectiles);
#endif
		}
		auto& tracers = world[crypt_str("tracers")]; {
			tracers[crypt_str("bullet_tracer")][crypt_str("size")] = vars.visuals.impacts_size;
			tracers[crypt_str("bullet_tracer")][crypt_str("enable")] = vars.visuals.bullet_tracer; {
				tracers[crypt_str("bullet_tracer")][crypt_str("sprite")] = vars.visuals.bullet_tracer_type;
				tracers[crypt_str("bullet_tracer")][crypt_str("color")][crypt_str("red")] = vars.visuals.bullet_tracer_color.get_red();
				tracers[crypt_str("bullet_tracer")][crypt_str("color")][crypt_str("green")] = vars.visuals.bullet_tracer_color.get_green();
				tracers[crypt_str("bullet_tracer")][crypt_str("color")][crypt_str("blue")] = vars.visuals.bullet_tracer_color.get_blue();
				tracers[crypt_str("bullet_tracer")][crypt_str("color")][crypt_str("alpha")] = vars.visuals.bullet_tracer_color.get_alpha();

				tracers[crypt_str("bullet_tracer")][crypt_str("local")] = vars.visuals.bullet_tracer_local;
				tracers[crypt_str("bullet_tracer")][crypt_str("local_color")][crypt_str("red")] = vars.visuals.bullet_tracer_local_color.get_red();
				tracers[crypt_str("bullet_tracer")][crypt_str("local_color")][crypt_str("green")] = vars.visuals.bullet_tracer_local_color.get_green();
				tracers[crypt_str("bullet_tracer")][crypt_str("local_color")][crypt_str("blue")] = vars.visuals.bullet_tracer_local_color.get_blue();
				tracers[crypt_str("bullet_tracer")][crypt_str("local_color")][crypt_str("alpha")] = vars.visuals.bullet_tracer_local_color.get_alpha();
			}
			
			tracers[crypt_str("bullet_impact")][crypt_str("enable")] = vars.visuals.bullet_impact; {
				tracers[crypt_str("bullet_impact")][crypt_str("color")][crypt_str("red")] = vars.visuals.bullet_impact_color.get_red();
				tracers[crypt_str("bullet_impact")][crypt_str("color")][crypt_str("green")] = vars.visuals.bullet_impact_color.get_green();
				tracers[crypt_str("bullet_impact")][crypt_str("color")][crypt_str("blue")] = vars.visuals.bullet_impact_color.get_blue();
				tracers[crypt_str("bullet_impact")][crypt_str("color")][crypt_str("alpha")] = vars.visuals.bullet_impact_color.get_alpha();
			}
		}
		auto& effects = world[crypt_str("effects")]; {
			
			
			for (int i = 0; i < 8; i++)
				effects[std::string(crypt_str("remove") + to_string(i)).c_str()] = vars.visuals.remove[i];
			effects[crypt_str("force_crosshair")] = vars.visuals.force_crosshair;
			effects[crypt_str("kill_effect")] = vars.visuals.kill_effect;
			effects[crypt_str("world_fov")] = vars.misc.worldfov;
			effects[crypt_str("viewmodel_fov")] = vars.misc.viewmodelfov;
			effects[crypt_str("recoil_crosshair")] = vars.visuals.recoil_crosshair;
			effects[crypt_str("radarhack")] = vars.visuals.radarhack;

			effects[crypt_str("nightmode")][crypt_str("enable")] = vars.visuals.nightmode; {
				effects[crypt_str("nightmode")][crypt_str("customize_color")] = vars.visuals.customize_color;
				effects[crypt_str("nightmode")][crypt_str("amount")] = vars.visuals.nightmode_amount;

				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("world")][crypt_str("red")] = vars.visuals.nightmode_color.get_red();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("world")][crypt_str("green")] = vars.visuals.nightmode_color.get_green();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("world")][crypt_str("blue")] = vars.visuals.nightmode_color.get_blue();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("world")][crypt_str("alpha")] = vars.visuals.nightmode_color.get_alpha();

				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("prop")][crypt_str("red")] = vars.visuals.nightmode_prop_color.get_red();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("prop")][crypt_str("green")] = vars.visuals.nightmode_prop_color.get_green();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("prop")][crypt_str("blue")] = vars.visuals.nightmode_prop_color.get_blue();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("prop")][crypt_str("alpha")] = vars.visuals.nightmode_prop_color.get_alpha();

				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("skybox")][crypt_str("red")] = vars.visuals.nightmode_skybox_color.get_red();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("skybox")][crypt_str("green")] = vars.visuals.nightmode_skybox_color.get_green();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("skybox")][crypt_str("blue")] = vars.visuals.nightmode_skybox_color.get_blue();
				effects[crypt_str("nightmode")][crypt_str("color")][crypt_str("skybox")][crypt_str("alpha")] = vars.visuals.nightmode_skybox_color.get_alpha();
			}
		}

		auto& chicken = world[crypt_str("chicken")]; {
			chicken[crypt_str("enable")] = vars.misc.chicken.enable;
			chicken[crypt_str("chickenbox")] = vars.misc.chicken.chickenbox;
			chicken[crypt_str("owner")] = vars.misc.chicken.owner;
			chicken[crypt_str("revealkiller")] = vars.misc.chicken.revealkiller;
			chicken[crypt_str("saykiller")] = vars.misc.chicken.saykiller;
			chicken[crypt_str("sidelist")] = vars.misc.chicken.sidelist;
			chicken[crypt_str("fish")] = vars.misc.chicken.fish;
			chicken[crypt_str("fishbox")] = vars.misc.chicken.fishbox;
			SaveColor(vars.misc.chicken.fishcolor, crypt_str("fishcolor"), &chicken);
		}
	}

	auto& misc = json[crypt_str("misc")]; {
		misc[crypt_str("anti_untrusted")] = vars.misc.antiuntrusted;
		misc[crypt_str("ublockorigin")] = vars.misc.ublockOrigin;
		misc[crypt_str("bunnyhop")] = vars.misc.bunnyhop;
		misc[crypt_str("autostrafe")] = vars.misc.autostrafe;
		misc[crypt_str("autostrafeonkey")] = vars.misc.autostrafeonkey;
		SaveBind(&vars.misc.autostrafekey, crypt_str("autostrafekey"), &misc);
		misc[crypt_str("knifebot")] = vars.misc.knifebot;
		misc[crypt_str("restrict_type")] = vars.misc.restrict_type;
		misc[crypt_str("clantag")] = vars.visuals.clantagspammer;
		misc[crypt_str("clantagtype")] = vars.visuals.clantagtype;
		misc[crypt_str("clantagcustomname")] = vars.visuals.clantagcustomname;
		misc[crypt_str("clantagrotating")] = vars.visuals.clantagrotating;
		misc[crypt_str("clantagspeed")] = vars.visuals.clantagspeed;
		misc[crypt_str("revealranks")] = vars.misc.revealranks;
		misc[crypt_str("playermodelct")] = vars.visuals.playermodelct;
		misc[crypt_str("playermodelt")] = vars.visuals.playermodelt;
		misc[crypt_str("menu_key")] = vars.menu.menu_key;
		misc[crypt_str("unlockinventoryaccess")] = vars.misc.unlockinventoryaccess;
		misc[crypt_str("window_opacity")] = vars.menu.window_opacity;

		misc[crypt_str("autobuy")][crypt_str("enable")] = vars.misc.autobuy.enable;
		misc[crypt_str("autobuy")][crypt_str("main")] = vars.misc.autobuy.main;
		misc[crypt_str("autobuy")][crypt_str("pistol")] = vars.misc.autobuy.pistol;
		for (int i = 0; i < 7; i++)
		{
			misc[crypt_str("autobuy_misc") + to_string(i)] = vars.misc.autobuy.misc[i];
		}

		misc[crypt_str("viewmodel")][crypt_str("enable")] = vars.misc.enableviewmodel;
		misc[crypt_str("viewmodel")][crypt_str("x")] = vars.misc.viewmodel_x;
		misc[crypt_str("viewmodel")][crypt_str("y")] = vars.misc.viewmodel_y;
		misc[crypt_str("viewmodel")][crypt_str("z")] = vars.misc.viewmodel_z;
		misc[crypt_str("viewmodel")][crypt_str("roll")] = vars.misc.viewmodel_roll;


		vars.menu.loaded_luas.clear();

		for (int i = 0; i < CLua::Get().scripts.size(); i++)
		{
			if (CLua::Get().loaded.at(i))
				vars.menu.loaded_luas.push_back(CLua::Get().scripts.at(i));
		}


		misc[crypt_str("loaded_luas_count")] = vars.menu.loaded_luas.size();
		for (int i = 0; i < vars.menu.loaded_luas.size(); i++)
		{
			misc[crypt_str("loaded_luas_") + to_string(i)] = vars.menu.loaded_luas.at(i).c_str();
		}
	}


	auto& movement = json[crypt_str("movement")]; {
		movement[crypt_str("edgebug")][crypt_str("enabled")] = vars.movement.edgebug.enabled;
		SaveBind(&vars.movement.edgebug.key, crypt_str("edgebug-key"), &movement);
		movement[crypt_str("edgebug")][crypt_str("ticks")] = vars.movement.edgebug.ticks;
		movement[crypt_str("edgebug")][crypt_str("lock")] = vars.movement.edgebug.lock;
		movement[crypt_str("edgebug")][crypt_str("locktype")] = vars.movement.edgebug.locktype;
		movement[crypt_str("edgebug")][crypt_str("marker")] = vars.movement.edgebug.marker;
		movement[crypt_str("edgebug")][crypt_str("detect_strafe")] = vars.movement.edgebug.detect_strafe;
		//movement[crypt_str("edgebug")][crypt_str("detect_strafe")] = vars.movement.edgebug.detect_strafe;
		SaveColor(vars.movement.edgebug.marker_color, crypt_str("marker_color"), &movement[crypt_str("edgebug")]);
		movement[crypt_str("edgebug")][crypt_str("marker_thickness")] = vars.movement.edgebug.marker_thickness;
		movement[crypt_str("jumpbug")] = vars.movement.jumpbug;
		SaveBind(&vars.movement.jumpbugkey, crypt_str("jumpbug-key"), &movement);
		movement[crypt_str("lengthcheck")] = vars.movement.lengthcheck;
		movement[crypt_str("jumpbugtype")] = vars.movement.jumpbugtype;
		movement[crypt_str("edgejump")] = vars.movement.edgejump;
		SaveBind(&vars.movement.edgejumpkey, crypt_str("edgejump-key"), &movement);
		movement[crypt_str("edgejump-ladder")] = vars.movement.ladderej;
		movement[crypt_str("longjump")] = vars.movement.longjump;
		SaveBind(&vars.movement.longjumpkey, crypt_str("longjump-key"), &movement);
		movement[crypt_str("minijump")] = vars.movement.minijump;
		SaveBind(&vars.movement.minijumpkey, crypt_str("minijump-key"), &movement);
		movement[crypt_str("showvelocity")] = vars.movement.showvelocity;
		movement[crypt_str("showtakeoff")] = vars.movement.showtakeoff;
		movement[crypt_str("showvelpos")] = vars.movement.showvelpos;
		movement[crypt_str("staticvelcolor")] = vars.movement.staticvelcolor;
		SaveColor(vars.movement.velcolor, crypt_str("velcolor"), &movement);
		movement[crypt_str("velocityfade")] = vars.movement.velocityfade;
		movement[crypt_str("velfadevelocity")] = vars.movement.velfadevelocity;
		movement[crypt_str("showveltype")] = vars.movement.showveltype;
		movement[crypt_str("ljstats")] = vars.movement.ljstats;
		movement[crypt_str("ebdetectmastertoggle")] = vars.movement.ebdetect.mastertoggle;
		movement[crypt_str("ebdetectsoundtype")] = vars.movement.ebdetect.soundtype;
		movement[crypt_str("ebdetecteffect")] = vars.movement.ebdetect.enabled[0];
		movement[crypt_str("ebdetectchat")] = vars.movement.ebdetect.enabled[1];
		movement[crypt_str("ebdetectcounter")] = vars.movement.ebdetect.enabled[2];
		movement[crypt_str("ebdetectsound")] = vars.movement.ebdetect.enabled[3];
		movement[crypt_str("ebdetectsoundname")] = vars.movement.ebdetect.sound_name;
		movement[crypt_str("autostrafetype")] = vars.movement.autostrafetype;
		movement[crypt_str("dxtostrafe")] = vars.movement.dxtostrafe;
		movement[crypt_str("msl")] = vars.movement.msl;
		movement[crypt_str("mslx")] = vars.movement.mslx;
		movement[crypt_str("fastduck")] = vars.movement.fastduck;
		movement[crypt_str("slidewalk")] = vars.movement.slidewalk;
		movement[crypt_str("autoduck")] = vars.movement.autoduck;
		movement[crypt_str("autoduckbefore")] = vars.movement.autoduckbefore;
		movement[crypt_str("autoduckafter")] = vars.movement.autoduckafter;
		movement[crypt_str("ladderglide")] = vars.movement.ladderglide;
		SaveBind(&vars.movement.ladderglidekey, crypt_str("ladderglidekey"), &movement);
		movement[crypt_str("autopixelsurf")] = vars.movement.autopixelsurf;
		SaveBind(&vars.movement.autopixelsurfkey, crypt_str("autopixelsurfkey"), &movement);
		movement[crypt_str("autopixelsurf_ticks")] = vars.movement.autopixelsurf_ticks;
		SaveBind(&vars.movement.autopixelsurf_align, crypt_str("autopixelsurf_aligner"), &movement);
		movement[crypt_str("autopixelsurf_freestand")] = vars.movement.autopixelsurf_freestand;
		movement[crypt_str("autopixelsurf_alignind")] = vars.movement.autopixelsurf_alignind;
		SaveColor(vars.movement.autopixelsurf_indcol1, crypt_str("autopixelsurf_indcol1"), &movement);
		
		movement[crypt_str("showstamina")] = vars.movement.showstamina;
		movement[crypt_str("staminascrpos")] = vars.movement.staminascrpos;
		movement[crypt_str("showstaminatype")] = vars.movement.showstaminatype;
		movement[crypt_str("staminatakeoff")] = vars.movement.staminatakeoff;
		SaveColor(vars.movement.staminacol, crypt_str("staminacol"), &movement);
		movement[crypt_str("staminafade")] = vars.movement.staminafade;
		movement[crypt_str("staminafadevel")] = vars.movement.staminafadevel;
		

		auto& graph = movement[crypt_str("graph")]; {
			graph[crypt_str("enabled")] = vars.movement.graph.enabled;
			graph[crypt_str("types0")] = vars.movement.graph.types[0];
			graph[crypt_str("types1")] = vars.movement.graph.types[1];
			SaveColor(vars.movement.graph.staminacol, crypt_str("staminacol"), &graph);
			SaveColor(vars.movement.graph.staminatextcol, crypt_str("staminatextcol"), &graph);
			graph[crypt_str("staminatofade")] = vars.movement.graph.staminatofade;
			graph[crypt_str("staminagaincolors")] = vars.movement.graph.staminagaincolors;
			SaveColor(vars.movement.graph.color, crypt_str("color"), &graph);
			SaveColor(vars.movement.graph.textcolor, crypt_str("textcolor"), &graph);
			graph[crypt_str("height")] = vars.movement.graph.height;
			graph[crypt_str("thickness")] = vars.movement.graph.thickness;
			graph[crypt_str("size")] = vars.movement.graph.size;
			graph[crypt_str("linewidth")] = vars.movement.graph.linewidth;
			graph[crypt_str("speed")] = vars.movement.graph.speed;
			graph[crypt_str("fadetype")] = vars.movement.graph.fadetype;
			graph[crypt_str("speedtofade")] = vars.movement.graph.speedtofade;
			graph[crypt_str("xscreenpos")] = vars.movement.graph.xscreenpos;
			graph[crypt_str("gaincolors")] = vars.movement.graph.gaincolors;
		}

		auto& recorder = movement[crypt_str("recorder")];
		{
			recorder[crypt_str("enabled")] = vars.movement.recorder.enabled;
			recorder[crypt_str("infowindow")] = vars.movement.recorder.infowindow;
			for (int i = 0; i < vars.movement.recorder.infowindowselected.size(); i++)
			{
				auto& curjson = recorder[crypt_str("infowindowselected")][i];	
				curjson[crypt_str("comp")] = vars.movement.recorder.infowindowselected.at(i);
			}
			recorder[crypt_str("smoothing")] = vars.movement.recorder.smoothing;
			recorder[crypt_str("maxrender")] = vars.movement.recorder.maxrender;
			SaveBind(&vars.movement.recorder.clearrecord, crypt_str("clearrecord"), &recorder);
			SaveBind(&vars.movement.recorder.startplayback, crypt_str("startplayback"), &recorder);
			SaveBind(&vars.movement.recorder.stopplayback, crypt_str("stopplayback"), &recorder);
			SaveBind(&vars.movement.recorder.startrecord, crypt_str("startrecord"), &recorder);
			SaveBind(&vars.movement.recorder.stoprecord, crypt_str("stoprecord"), &recorder);
			recorder[crypt_str("lockva")] = vars.movement.recorder.lockva;
			recorder[crypt_str("ringsize")] = vars.movement.recorder.ringsize;
			SaveColor(vars.movement.recorder.startring, crypt_str("startring"), &recorder);
			SaveColor(vars.movement.recorder.endring, crypt_str("endring"), &recorder);
			SaveColor(vars.movement.recorder.linecolor, crypt_str("linecolor"), &recorder);
			SaveColor(vars.movement.recorder.textcolor, crypt_str("textcolor"), &recorder);
		}

		auto& checkpoint = movement[crypt_str("checkpoint")];
		{
			checkpoint[crypt_str("enabled")] = vars.movement.checkpoint.enabled;
			SaveBind(&vars.movement.checkpoint.teleportkey, crypt_str("teleportkey"), &checkpoint);
			SaveBind(&vars.movement.checkpoint.checkpointkey, crypt_str("checkpointkey"), &checkpoint);
			SaveBind(&vars.movement.checkpoint.nextkey, crypt_str("nextkey"), &checkpoint);
			SaveBind(&vars.movement.checkpoint.prevkey, crypt_str("prevkey"), &checkpoint);
			SaveBind(&vars.movement.checkpoint.undokey, crypt_str("undokey"), &checkpoint);
			SaveColor(vars.movement.checkpoint.color, crypt_str("color"), &checkpoint);
		}

		auto& indicators = movement[crypt_str("indicators")];
		{
			indicators[crypt_str("show_active")] = vars.movement.indicators.show_active;
			SaveColor(vars.movement.indicators.active_col, crypt_str("active_col"), &indicators);
			SaveColor(vars.movement.indicators.passive_col, crypt_str("passive_col"), &indicators);
			indicators[crypt_str("height")] = vars.movement.indicators.height;
			for (int i = 0; i < 8; i++)
			{
				auto& curjson = indicators[crypt_str("on")][i];
				curjson[crypt_str("ind")] = vars.movement.indicators.on[i];
			}
		}
	}
	
	auto& skinchanger = json[crypt_str("skinchanger")];
	{
		skinchanger[crypt_str("itemssize")] = g_config.get_items().size();
		for (int i = 0; i < g_config.get_items().size(); i++)
		{
			auto& skinchangerweapon = skinchanger[crypt_str("skinweapon")][i];
			auto& curweapon = g_config.get_items().at(i);
			skinchangerweapon[crypt_str("custom_name")] = curweapon.custom_name;
			skinchangerweapon[crypt_str("definition_index")] = curweapon.definition_index;
			skinchangerweapon[crypt_str("definition_override_index")] = curweapon.definition_override_index;
			skinchangerweapon[crypt_str("definition_override_vector_index")] = curweapon.definition_override_vector_index;
			skinchangerweapon[crypt_str("definition_vector_index")] = curweapon.definition_vector_index;
			skinchangerweapon[crypt_str("enabled")] = curweapon.enabled;
			skinchangerweapon[crypt_str("entity_quality_index")] = curweapon.entity_quality_index;
			skinchangerweapon[crypt_str("entity_quality_vector_index")] = curweapon.entity_quality_vector_index;
			skinchangerweapon[crypt_str("name")] = curweapon.name;
			skinchangerweapon[crypt_str("paint_kit_index")] = curweapon.paint_kit_index;
			skinchangerweapon[crypt_str("paint_kit_vector_index")] = curweapon.paint_kit_vector_index;
			skinchangerweapon[crypt_str("seed")] = curweapon.seed;
			skinchangerweapon[crypt_str("colors0")] = curweapon.colors[0];
			skinchangerweapon[crypt_str("colors1")] = curweapon.colors[1];
			skinchangerweapon[crypt_str("colors2")] = curweapon.colors[2];
			skinchangerweapon[crypt_str("colors3")] = curweapon.colors[3];
			skinchangerweapon[crypt_str("pearlescent")] = curweapon.pearlescent;
			skinchangerweapon[crypt_str("stat_trak")] = curweapon.stat_trak;
			skinchangerweapon[crypt_str("wear")] = curweapon.wear;
		}
		
	}

	for (size_t i = 0; i < vars.chams.size(); i++) {
		auto& chamsJson = json[crypt_str("Chams")][i];
		const auto& chamsConfig = vars.chams[i];

		for (size_t j = 0; j < chamsConfig.materials.size(); j++) {
			auto& materialsJson = chamsJson[j];
			const auto& materialsConfig = vars.chams[i].materials[j];

			materialsJson[crypt_str("Enabled")] = materialsConfig.enabled;
			materialsJson[crypt_str("Health based")] = materialsConfig.healthBased;
			materialsJson[crypt_str("Blinking")] = materialsConfig.blinking;
			materialsJson[crypt_str("Material")] = materialsConfig.material;
			materialsJson[crypt_str("Wireframe")] = materialsConfig.wireframe;

			{
				auto& colorJson = materialsJson[crypt_str("Color")];
				const auto& colorConfig = materialsConfig; // leftover

				colorJson[crypt_str("Color")][0] = colorConfig.color[0];
				colorJson[crypt_str("Color")][1] = colorConfig.color[1];
				colorJson[crypt_str("Color")][2] = colorConfig.color[2];
				colorJson[crypt_str("Color")][3] = colorConfig.color[3];

				colorJson[crypt_str("Rainbow")] = colorConfig.rainbow;
				colorJson[crypt_str("Rainbow speed")] = colorConfig.rainbowSpeed;
			}
		}
	}

	


	static TCHAR path[256];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + crypt_str("\\Reflect\\");
		file = std::string(path) + crypt_str("\\Reflect\\") + cfg_name;
	}

	CreateDirectory(folder.c_str(), NULL);

	auto str = configuration.toStyledString();
	ByteArray vec_enc = ByteArray(str.begin(), str.end());
	xor_crypt(vec_enc);
	auto str_enc = std::string(vec_enc.begin(), vec_enc.end());

	std::ofstream file_out(file);
	if (file_out.good())
		file_out << str_enc;
	file_out.close();
	Msg(std::string(crypt_str("Saved Config ") + cfg_name), vars.visuals.eventlog_color);
}
ByteArray ReadAllBytes(char const* filename)
{
	ifstream ifs(filename, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	ByteArray result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read((char*)&result[0], pos);

	return result;
}
void CConfig::Load(string cfg_name)
{
	json_t configuration;
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + crypt_str("\\Reflect\\");
		file = std::string(path) + crypt_str("\\Reflect\\") + cfg_name;
	}
	CreateDirectory(folder.c_str(), NULL);
	//std::ifstream file_out(file);
	ByteArray vec_enc;
	//if (file_out.good())
	//	file_out >> configuration;

	vec_enc = ReadAllBytes(file.c_str());
	
	if (vec_enc.empty()) {
		Msg(crypt_str("Empty cfg cant be loaded"), vars.visuals.eventlog_color);
		return;
	}
	
	//auto vec_dec = bXor::decrypt(vec_enc, xs64_extp_key);
	ByteArray vec_dec = vec_enc;
	xor_crypt(vec_dec);
	std::stringstream stream_dec;
	auto str_dec = std::string(vec_dec.begin(), vec_dec.end());
	stream_dec << str_dec;
	stream_dec >> configuration;

	if (!configuration.isMember(crypt_str("config"))) {
		Msg(crypt_str("Unable to load cfg"), vars.visuals.eventlog_color);
		return;
	}

	auto& json = configuration[crypt_str("config")];
	//json[crypt_str("name")] = cfg_name;
	if (json.isMember(crypt_str("name"))) {
		if (json[crypt_str("name")] != cfg_name) {
			Msg(crypt_str("Cfg name warning | old name: ") + json[crypt_str("name")].asString() + crypt_str("| new name: ") + cfg_name, vars.visuals.eventlog_color);
			//return;
		}
	}
	else {
		Msg(crypt_str("Unable to find cfg with name: ") + json[crypt_str("name")].asString() + crypt_str(""), vars.visuals.eventlog_color);
		return;
	}

	LoadColor(&menu_colors::main_color, crypt_str("main_color"), json);
	LoadColor(&menu_colors::background1, crypt_str("background1"), json);
	LoadColor(&menu_colors::background2, crypt_str("background2"), json);

	auto& legitbot = json[crypt_str("legitbot")]; {
		LoadBool(&vars.legitbot.aimbot, crypt_str("aimbot"), legitbot);
		LoadBool(&vars.legitbot.autopistol, crypt_str("autopistol"), legitbot);
		LoadBool(&vars.legitbot.backtrack, crypt_str("backtrack"), legitbot);
		LoadFloat(&vars.legitbot.btlength, crypt_str("btlength"), legitbot);
		LoadBool(&vars.legitbot.onkey, crypt_str("onkey"), legitbot);
		LoadBind(&vars.legitbot.key, crypt_str("key"), legitbot);
		LoadBool(&vars.legitbot.teammates, crypt_str("teammates"), legitbot);
		LoadBool(&vars.legitbot.rcsstandalone, crypt_str("rcsstandalone"), legitbot);
		for (int i = 0; i < vars.legitbot.weapons.size(); i++)
		{
			auto& weapon = legitbot[crypt_str("weapon")][i];
			LoadBool(&vars.legitbot.weapons[i].aimbacktrack, crypt_str("aimbacktrack"), weapon);
			LoadBool(&vars.legitbot.weapons[i].aimlock, crypt_str("aimlock"), weapon);
			LoadBool(&vars.legitbot.weapons[i].silent, crypt_str("silent"), weapon);
			LoadBool(&vars.legitbot.weapons[i].enabled, crypt_str("enabled"), weapon);
			LoadFloat(&vars.legitbot.weapons[i].fov, crypt_str("fov"), weapon);
			LoadBool(&vars.legitbot.weapons[i].hitboxes[0], crypt_str("hitbox0"), weapon);
			LoadBool(&vars.legitbot.weapons[i].hitboxes[1], crypt_str("hitbox1"), weapon);
			LoadBool(&vars.legitbot.weapons[i].hitboxes[2], crypt_str("hitbox2"), weapon);
			LoadBool(&vars.legitbot.weapons[i].hitboxes[3], crypt_str("hitbox3"), weapon);
			LoadBool(&vars.legitbot.weapons[i].hitboxes[4], crypt_str("hitbox4"), weapon);
			LoadBool(&vars.legitbot.weapons[i].ignoreflash, crypt_str("ignoreflash"), weapon);
			LoadBool(&vars.legitbot.weapons[i].ignoresmoke, crypt_str("ignoresmoke"), weapon);
			LoadFloat(&vars.legitbot.weapons[i].rcsx, crypt_str("rcsx"), weapon);
			LoadFloat(&vars.legitbot.weapons[i].rcsy, crypt_str("rcsy"), weapon);
			LoadBool(&vars.legitbot.weapons[i].scopedonly, crypt_str("scopedonly"), weapon);
			LoadFloat(&vars.legitbot.weapons[i].smooth, crypt_str("smooth"), weapon);
		}

		auto& triggerbot = legitbot[crypt_str("triggerbot")]; {
			LoadBool(&vars.legitbot.triggerbot.enabled, crypt_str("enabled"), triggerbot);
			LoadBool(&vars.legitbot.triggerbot.onkey, crypt_str("onkey"), triggerbot);
			LoadBind(&vars.legitbot.triggerbot.trigkey, crypt_str("trigkey"), triggerbot);
			LoadFloat(&vars.legitbot.triggerbot.delay, crypt_str("delay"), triggerbot);
			LoadInt(&vars.legitbot.triggerbot.mindmg, crypt_str("mindmg"), triggerbot);
			for (int i = 0; i < 5; i++)
			{
				auto& curhitbox = triggerbot[crypt_str("hitboxes")][i];
				LoadBool(&vars.legitbot.triggerbot.hitboxes[i], crypt_str("box"), curhitbox);
			}
			LoadBool(&vars.legitbot.triggerbot.checkflash, crypt_str("checkflash"), triggerbot);
			LoadBool(&vars.legitbot.triggerbot.checksmoke, crypt_str("checksmoke"), triggerbot);
			LoadBool(&vars.legitbot.triggerbot.scopedonly, crypt_str("scopedonly"), triggerbot);
			LoadBool(&vars.legitbot.triggerbot.teammates, crypt_str("teammates"), triggerbot);
			LoadBool(&vars.legitbot.triggerbot.shootatbacktrack, crypt_str("shootatbacktrack"), triggerbot);
		}
	}
	
	auto& ragebot = json[crypt_str("ragebot")]; {
		LoadBool(&vars.ragebot.enable, crypt_str("enable"), ragebot);
		LoadBool(&vars.ragebot.onkey, crypt_str("rage_onkey"), ragebot);
		LoadBind(&vars.ragebot.key, crypt_str("rage_key"), ragebot);
		LoadBool(&vars.ragebot.autoshoot, crypt_str("auto_shoot"), ragebot);
		LoadBool(&vars.ragebot.silent, crypt_str("silent"), ragebot);
		LoadInt(&vars.ragebot.fov, crypt_str("fov"), ragebot);
		LoadBool(&vars.ragebot.autoscope, crypt_str("auto_scope"), ragebot);
		LoadBool(&vars.ragebot.resolver, crypt_str("resolver"), ragebot);
		LoadBool(&vars.ragebot.zeusbot, crypt_str("zeusbot"), ragebot);
		LoadInt(&vars.ragebot.zeuschance, crypt_str("zeuschance"), ragebot);
		//LoadBool(&vars.ragebot.shotrecord, crypt_str("shot_record"), ragebot);
		LoadBind(&vars.ragebot.override_dmg, crypt_str("override_dmg"), ragebot);
		LoadBind(&vars.ragebot.force_safepoint, crypt_str("force_safepoint"), ragebot);
		LoadBind(&vars.ragebot.baim, crypt_str("baim"), ragebot);
		LoadInt(&vars.ragebot.baim_type, crypt_str("baim_type"), ragebot);
		LoadBool(&vars.ragebot.dt_teleport, crypt_str("dt_teleport"), ragebot);
		LoadBool(&vars.ragebot.antiexploit, crypt_str("antiexploit"), ragebot);
		LoadBool(&vars.ragebot.hideshot, crypt_str("hideshot"), ragebot);
		LoadBind(&vars.ragebot.double_tap, crypt_str("double_tap"), ragebot);
		LoadBind(&vars.ragebot.hideShots, crypt_str("hideShots"), ragebot);
		//LoadBool(&vars.ragebot.extended_backtrack, crypt_str("extended_backtrack"), ragebot);

		//LoadColor(&vars.ragebot.shot_clr, crypt_str("shot_clr"), ragebot);

		auto& weapon_cfg = ragebot[crypt_str("weapon_cfg")];
		for (int i = 0; i < 7; i++) {
			string category = crypt_str("weapon_") + std::to_string(i);
			LoadBool(&vars.ragebot.weapon[i].enable, crypt_str("enable"), weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].mindamage, crypt_str("mindamage"), weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].mindamage_override, crypt_str("mindamage_override"), weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].hitchance, crypt_str("hitchance"), weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].doubletap_hc, crypt_str("doubletap_hc"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].multipoint, crypt_str("multipoint"), weapon_cfg[category]);
			//LoadBool(&vars.ragebot.weapon[i].quickstop, crypt_str("quickstop"), weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].autostopMode, crypt_str("autostopMode"), weapon_cfg[category]);
			//LoadInt(&vars.ragebot.weapon[i].quickstop_options, crypt_str("quickstop_options"), weapon_cfg[category]);
			//LoadUInt(&vars.ragebot.weapon[i].hitscan, crypt_str("hitboxes"), weapon_cfg[category]);

			LoadBool(&vars.ragebot.weapon[i].hitboxes[0], crypt_str("hitbox0"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[1], crypt_str("hitbox1"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[2], crypt_str("hitbox2"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[3], crypt_str("hitbox3"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[4], crypt_str("hitbox4"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[5], crypt_str("hitbox5"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[6], crypt_str("hitbox6"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[7], crypt_str("hitbox7"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[8], crypt_str("hitbox8"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[9], crypt_str("hitbox9"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].hitboxes[10], crypt_str("hitbox10"), weapon_cfg[category]);


			LoadBool(&vars.ragebot.weapon[i].static_scale, crypt_str("static_scale"), weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].scale_body, crypt_str("scale_body"), weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].scale_head, crypt_str("scale_head"), weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].prefer_safepoint, crypt_str("prefer_safepoint"), weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].max_misses, crypt_str("max_misses"), weapon_cfg[category]);
		}
	}

	auto& antiaim = json[crypt_str("antiaim")]; {
		LoadBool(&vars.antiaim.enable, crypt_str("enable"), antiaim);
		LoadBool(&vars.antiaim.shotDesync, crypt_str("shotDesync"), antiaim);
		LoadBool(&vars.antiaim.aa_on_use, crypt_str("aa_on_use"), antiaim);
		LoadInt(&vars.antiaim.pitch, crypt_str("pitch"), antiaim);
		LoadInt(&vars.antiaim.jitter_angle, crypt_str("jitter_angle"), antiaim);
		LoadInt(&vars.antiaim.zlean, crypt_str("zlean"), antiaim);
		LoadBool(&vars.antiaim.zleanenable, crypt_str("zleanenable"), antiaim);
		LoadBool(&vars.antiaim.attarget, crypt_str("at_target"), antiaim);
		LoadBool(&vars.antiaim.attarget_off_when_offsreen, crypt_str("attarget_off_when_offsreen"), antiaim);

		LoadBind(&vars.antiaim.fakeduck, crypt_str("fakeduck"), antiaim);
		LoadBind(&vars.antiaim.slowwalk, crypt_str("slowwalk"), antiaim);
		LoadInt(&vars.antiaim.slowwalkspeed, crypt_str("slowwalkspeed"), antiaim);

		LoadBind(&vars.antiaim.inverter, crypt_str("inverter"), antiaim);
		LoadBool(&vars.antiaim.static_delta, crypt_str("static_delta"), antiaim);	
		LoadBool(&vars.antiaim.avoid_overlap, crypt_str("avoid_overlap"), antiaim);
		LoadInt(&vars.antiaim.desync_amount, crypt_str("desync_amount"), antiaim);
		LoadInt(&vars.antiaim.base_yaw, crypt_str("base_yaw"), antiaim);
		LoadBool(&vars.antiaim.simtimeStopper, crypt_str("simtimeStopper"), antiaim);

		LoadBool(&vars.antiaim.manual_antiaim, crypt_str("manual_antiaim"), antiaim);
		LoadBool(&vars.antiaim.ignore_attarget, crypt_str("ignore_attarget"), antiaim);

		LoadBind(&vars.antiaim.manual_left, crypt_str("manual_left"), antiaim);
		LoadBind(&vars.antiaim.manual_right, crypt_str("manual_right"), antiaim);
		LoadBind(&vars.antiaim.manual_back, crypt_str("manual_back"), antiaim);
		LoadBind(&vars.antiaim.manual_forward, crypt_str("manual_forward"), antiaim);

		auto& fakelag = antiaim[crypt_str("fakelag")]; {
			LoadInt(&vars.antiaim.fakelag, crypt_str("type"), fakelag);
			LoadBool(&vars.antiaim.fakelag_when_standing, crypt_str("when_standing"), fakelag);
			LoadBool(&vars.antiaim.fakelag_when_exploits, crypt_str("when_exploits"), fakelag);
			LoadInt(&vars.antiaim.fakelagfactor, crypt_str("factor"), fakelag);
			LoadInt(&vars.antiaim.fakelagvariance, crypt_str("switch_ticks"), fakelag);
			LoadInt(&vars.antiaim.fakelagvariance, crypt_str("breakLC"), fakelag);
		}
		LoadBool(&vars.antiaim.extendlby, crypt_str("extend_lby"), antiaim);
		LoadBind(&vars.antiaim.fakehead, crypt_str("fakehead"), antiaim);
		LoadInt(&vars.antiaim.fakehead_mode, crypt_str("fakehead_mode"), antiaim);
	}

	auto& players = json[crypt_str("players")]; {
		auto& esp = players[crypt_str("esp")];

		LoadBool(&vars.visuals.enable, crypt_str("enable"), esp);
		LoadBool(&vars.visuals.dormant, crypt_str("dormant"), esp);
		LoadBool(&vars.visuals.zeus_warning, crypt_str("zeus_warning"), esp);
		LoadBool(&vars.visuals.skeleton, crypt_str("skeleton"), esp);
		LoadColor(&vars.visuals.skeleton_color, crypt_str("skeleton_color"), esp);
		LoadColor(&vars.visuals.skeleton_color_occluded, crypt_str("skeleton_color_occluded"), esp);
		LoadBool(&vars.visuals.box, crypt_str("box"), esp);
		LoadColor(&vars.visuals.box_color, crypt_str("box_color"), esp);
		LoadColor(&vars.visuals.box_color_occluded, crypt_str("box_color_occluded"), esp);
		LoadBool(&vars.visuals.healthbar, crypt_str("enable"), esp[crypt_str("healthbar")]);
		LoadBool(&vars.visuals.override_hp, crypt_str("override_hp"), esp[crypt_str("healthbar")]);
		LoadColor(&vars.visuals.hp_color, crypt_str("hp_color"), esp[crypt_str("healthbar")]);
		LoadColor(&vars.visuals.hp_color_occluded, crypt_str("hp_color_occluded"), esp);

		LoadBool(&vars.visuals.name, crypt_str("name"), esp);
		LoadColor(&vars.visuals.name_color, crypt_str("name_color"), esp);
		LoadColor(&vars.visuals.name_color_occluded, crypt_str("name_color_occluded"), esp);

		LoadBool(&vars.visuals.weapon, crypt_str("weapon"), esp);
		LoadColor(&vars.visuals.name_color, crypt_str("name_color"), esp);
		LoadColor(&vars.visuals.name_color_occluded, crypt_str("name_color_occluded"), esp);

		LoadBool(&vars.visuals.weaponicontype[0], crypt_str("weaponicontype0"), esp);
		LoadBool(&vars.visuals.weaponicontype[1], crypt_str("weaponicontype1"), esp);

		LoadBool(&vars.visuals.ammo, crypt_str("ammo"), esp);
		LoadColor(&vars.visuals.ammo_color, crypt_str("ammo_color"), esp);
		LoadColor(&vars.visuals.ammo_color_occluded, crypt_str("ammo_color_occluded"), esp);

		LoadUInt(&vars.visuals.flags, crypt_str("flags"), esp);
		LoadColor(&vars.visuals.flags_color, crypt_str("flags_color"), esp);
		LoadColor(&vars.visuals.flags_color_occluded, crypt_str("flags_color_occluded"), esp);
		LoadBool(&vars.visuals.shot_multipoint, crypt_str("show_multipoint"), esp);
		LoadBool(&vars.visuals.out_of_fov, crypt_str("enable"), esp[crypt_str("out_of_fov")]);
		LoadInt(&vars.visuals.out_of_fov_distance, crypt_str("distance"), esp[crypt_str("out_of_fov")]);
		LoadInt(&vars.visuals.out_of_fov_size, crypt_str("size"), esp[crypt_str("out_of_fov")]);
		LoadColor(&vars.visuals.out_of_fov_color, crypt_str("color"), esp[crypt_str("out_of_fov")]);
		LoadBool(&vars.visuals.out_of_fov_drawvisible, crypt_str("drawvisible"), esp[crypt_str("out_of_fov")]);
		LoadColor(&vars.visuals.out_of_fov_visiblecolor, crypt_str("visiblecolor"), esp[crypt_str("out_of_fov")]);

		LoadBool(&vars.visuals.dlights, crypt_str("dlights"), esp);
		LoadColor(&vars.visuals.dlights_color, crypt_str("dlights_color"), esp);
		LoadColor(&vars.visuals.dlights_color_occluded, crypt_str("dlights_color_occluded"), esp);

		auto& models = players[crypt_str("models")]; {  // not in use anymore
			auto& chams_cfg = models[crypt_str("chams")]; {
				LoadBool(&vars.visuals.ragdoll_force, crypt_str("ragdoll_force"), chams_cfg);

				LoadBool(&vars.visuals.chams, crypt_str("enable"), chams_cfg[crypt_str("enemy")]);
				LoadBool(&vars.visuals.chamsxqz, crypt_str("through_walls"), chams_cfg[crypt_str("enemy")]);
				LoadBool(&vars.visuals.chams_ragdoll, crypt_str("chams_ragdoll"), chams_cfg[crypt_str("enemy")]);
				LoadColor(&vars.visuals.chamscolor, crypt_str("visible_color"), chams_cfg[crypt_str("enemy")]);
				LoadColor(&vars.visuals.chamscolor_xqz, crypt_str("through_walls_color"), chams_cfg[crypt_str("enemy")]);
				LoadColor(&vars.visuals.glow_col, crypt_str("overlay_color"), chams_cfg[crypt_str("enemy")]);
				LoadColor(&vars.visuals.glow_col_xqz, crypt_str("xqz_overlay_color"), chams_cfg[crypt_str("enemy")]);
				LoadInt(&vars.visuals.overlay, crypt_str("overlay"), chams_cfg[crypt_str("enemy")]);
				LoadInt(&vars.visuals.overlay_xqz, crypt_str("overlay_xqz"), chams_cfg[crypt_str("enemy")]);
				LoadInt(&vars.visuals.chamstype, crypt_str("material"), chams_cfg[crypt_str("enemy")]);
				LoadInt(&vars.visuals.phong_exponent, crypt_str("phong_exponent"), chams_cfg[crypt_str("enemy")]);
				LoadInt(&vars.visuals.phong_boost, crypt_str("phong_boost"), chams_cfg[crypt_str("enemy")]);
				LoadInt(&vars.visuals.rim, crypt_str("rim"), chams_cfg[crypt_str("enemy")]);
				LoadInt(&vars.visuals.chams_brightness, crypt_str("brightness"), chams_cfg[crypt_str("enemy")]);
				LoadInt(&vars.visuals.pearlescent, crypt_str("pearlescent"), chams_cfg[crypt_str("enemy")]);

				LoadColor(&vars.visuals.metallic_clr, crypt_str("metallic_color"), chams_cfg[crypt_str("enemy")]);
				LoadColor(&vars.visuals.metallic_clr2, crypt_str("phong_color"), chams_cfg[crypt_str("enemy")]);

				LoadBool(&vars.visuals.localchams, crypt_str("enable"), chams_cfg[crypt_str("local")]); {
					LoadBool(&vars.visuals.blend_on_scope, crypt_str("blend_on_scope"), chams_cfg[crypt_str("local")]);
					LoadInt(&vars.visuals.blend_value, crypt_str("blend_value"), chams_cfg[crypt_str("local")]);
					LoadBool(&vars.visuals.localchams, crypt_str("enable"), chams_cfg[crypt_str("local")]);
					LoadColor(&vars.visuals.localchams_color, crypt_str("visible_color"), chams_cfg[crypt_str("local")]);
					LoadColor(&vars.visuals.local_glow_color, crypt_str("overlay_color"), chams_cfg[crypt_str("local")]);
					LoadInt(&vars.visuals.localchamstype, crypt_str("material"), chams_cfg[crypt_str("local")]);
					LoadInt(&vars.visuals.local_chams.phong_exponent, crypt_str("phong_exponent"), chams_cfg[crypt_str("local")]);
					LoadInt(&vars.visuals.local_chams.phong_boost, crypt_str("phong_boost"), chams_cfg[crypt_str("local")]);
					LoadInt(&vars.visuals.local_chams.rim, crypt_str("rim"), chams_cfg[crypt_str("local")]);
					LoadInt(&vars.visuals.local_chams_brightness, crypt_str("brightness"), chams_cfg[crypt_str("local")]);
					LoadInt(&vars.visuals.local_chams.pearlescent, crypt_str("pearlescent"), chams_cfg[crypt_str("local")]);
					LoadInt(&vars.visuals.local_chams.overlay, crypt_str("overlay"), chams_cfg[crypt_str("local")]);

					LoadColor(&vars.visuals.local_chams.metallic_clr, crypt_str("metallic_color"), chams_cfg[crypt_str("local")]);
					LoadColor(&vars.visuals.local_chams.metallic_clr2, crypt_str("phong_color"), chams_cfg[crypt_str("local")]);
				}

				LoadBool(&vars.visuals.interpolated_bt, crypt_str("interpolated"), chams_cfg[crypt_str("misc_chams_2")]);
				LoadBool(&vars.visuals.interpolated_dsy, crypt_str("interpolated"), chams_cfg[crypt_str("misc_chams_3")]);

				for (int i = 0; i < 5; i++) {
					string category = crypt_str("misc_chams_") + std::to_string(i);
					LoadBool(&vars.visuals.misc_chams[i].enable, crypt_str("enable"), chams_cfg[category]);
					LoadColor(&vars.visuals.misc_chams[i].clr, crypt_str("model_color"), chams_cfg[category]);
					LoadColor(&vars.visuals.misc_chams[i].glow_clr, crypt_str("overlay_color"), chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].material, crypt_str("material"), chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].phong_exponent, crypt_str("phong_exponent"), chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].phong_boost, crypt_str("phong_boost"), chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].rim, crypt_str("rim"), chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].chams_brightness, crypt_str("brightness"), chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].pearlescent, crypt_str("pearlescent"), chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].overlay, crypt_str("overlay"), chams_cfg[category]);

					LoadColor(&vars.visuals.misc_chams[i].metallic_clr, crypt_str("metallic_color"), chams_cfg[category]);
					LoadColor(&vars.visuals.misc_chams[i].metallic_clr2, crypt_str("phong_color"), chams_cfg[category]);
				}
			}
			auto& glow = models[crypt_str("glow")]; {
				LoadInt(&vars.visuals.glowtype, crypt_str("style"), glow);
				LoadBool(&vars.visuals.glow, crypt_str("enable"), glow[crypt_str("enemy")]);
				LoadColor(&vars.visuals.glow_color, crypt_str("color"), glow[crypt_str("enemy")]);
				LoadColor(&vars.visuals.glow_color_occluded, crypt_str("glow_color_occluded"), glow);

				LoadBool(&vars.visuals.local_glow, crypt_str("enable"), glow[crypt_str("local")]);
				LoadColor(&vars.visuals.local_glow_clr, crypt_str("color"), glow[crypt_str("local")]);
			}
		}

		auto& misc = players[crypt_str("misc")]; {
			LoadInt(&vars.visuals.thirdperson_dist, crypt_str("distance"), misc[crypt_str("thirdperson")]);
			LoadBind(&vars.misc.thirdperson, crypt_str("bind"), misc[crypt_str("thirdperson")]);

			LoadBind(&vars.misc.peek_assist, crypt_str("peek_assist"), misc);
			LoadBind(&vars.misc.blockbot, crypt_str("blockbot"), misc);

			LoadBool(&vars.visuals.indicators, crypt_str("indicators"), misc);
			LoadBool(&vars.visuals.indicators_rage, crypt_str("indicators_rage"), misc);
			LoadBool(&vars.visuals.antiaim_arrows, crypt_str("antiaim_arrows"), misc);
			LoadColor(&vars.visuals.antiaim_arrows_color, crypt_str("antiaim_arrows_color"), misc);
			LoadInt(&vars.visuals.aspect_ratio, crypt_str("aspect_ratio"), misc);
			LoadBool(&vars.visuals.watermark, crypt_str("watermark"), misc);
			LoadBool(&vars.misc.sv_purebypass, crypt_str("svpure"), misc);
			LoadBool(&vars.visuals.speclist, crypt_str("speclist"), misc);
			LoadFloat(&vars.visuals.speclistx, crypt_str("speclistx"), misc);
			LoadFloat(&vars.visuals.speclisty, crypt_str("speclisty"), misc);
			LoadFloat(&vars.visuals.keybindx, crypt_str("keybindx"), misc);
			LoadFloat(&vars.visuals.keybindy, crypt_str("keybindy"), misc);
			LoadFloat(&vars.movement.recorder.infowindowx, crypt_str("infowindowx"), misc);
			LoadFloat(&vars.movement.recorder.infowindowy, crypt_str("infowindowy"), misc);
			LoadFloat(&vars.movement.checkpoint.teleportx, crypt_str("teleportx"), misc);
			LoadFloat(&vars.movement.checkpoint.teleporty, crypt_str("teleporty"), misc);
			

			LoadBool(&vars.visuals.teamdmglist, crypt_str("teamdmglist"), misc);
			LoadFloat(&vars.visuals.teamdmgx, crypt_str("teamdmgx"), misc);
			LoadFloat(&vars.visuals.teamdmgy, crypt_str("teamdmgy"), misc);
			LoadBool(&vars.visuals.cooldownsay, crypt_str("cooldownsay"), misc);
			LoadBool(&vars.visuals.addcounter, crypt_str("addcounter"), misc);
			std::string coolsaytxt;
			LoadStr(&coolsaytxt, crypt_str("cooldownsaytext"), misc);
			strcpy_s(vars.visuals.cooldownsaytext, 32, coolsaytxt.c_str());
			LoadBool(&vars.misc.autoaccept, crypt_str("autoaccept"), misc);
			LoadFloat(&vars.misc.autoacceptdelay, crypt_str("autoacceptdelay"), misc);
			LoadBool(&vars.misc.killsay, crypt_str("killsay"), misc);
			LoadBool(&vars.visuals.eventlog, crypt_str("enable"), misc[crypt_str("eventlog")]);
			LoadColor(&vars.visuals.eventlog_color, crypt_str("color"), misc[crypt_str("eventlog")]);
			LoadBool(&vars.visuals.print_votes, crypt_str("print_votes"), misc[crypt_str("eventlog")]);
			LoadInt(&vars.visuals.print_votes_chat, crypt_str("print_votes_chat"), misc[crypt_str("eventlog")]);

	
			LoadBool(&vars.visuals.hitmarker, crypt_str("enable"), misc[crypt_str("hitmarker")]);
			LoadBool(&vars.visuals.visualize_damage, crypt_str("visualize_damage"), misc[crypt_str("hitmarker")]);
			LoadBool(&vars.visuals.hitmarker_sound, crypt_str("enable_sound"), misc[crypt_str("hitmarker")]);
			LoadInt(&vars.visuals.hitmarker_sound_type, crypt_str("hitmarker_sound_type"), misc[crypt_str("hitmarker")]);
			LoadColor(&vars.visuals.hitmarker_color, crypt_str("color"), misc[crypt_str("hitmarker")]);
			std::string bb;
			LoadStr(&bb, crypt_str("hitmarker_name"), misc[crypt_str("hitmarker")]);
			strcpy_s(vars.visuals.hitmarker_name, 32, bb.c_str());

			LoadBool(&vars.visuals.preverse_killfeed, crypt_str("enable"), misc[crypt_str("preverse_killfeed")]);

			LoadBool(&vars.visuals.nadepred, crypt_str("enable"), misc[crypt_str("nadepred")]);
			LoadColor(&vars.visuals.nadepred_color, crypt_str("color"), misc[crypt_str("nadepred")]);
			LoadInt(&vars.visuals.taser_range, crypt_str("mode"), misc[crypt_str("taser_range")]);
			LoadColor(&vars.visuals.taser_range_color, crypt_str("color"), misc[crypt_str("taser_range")]);
			LoadBool(&vars.visuals.spotifysong, crypt_str("spotifysong"), misc);
			LoadBool(&vars.visuals.sunset_enabled, crypt_str("sunset_enabled"), misc);
			LoadFloat(&vars.visuals.rot_x, crypt_str("rot_x"), misc);
			LoadFloat(&vars.visuals.rot_y, crypt_str("rot_y"), misc);
			LoadInt(&vars.visuals.rot_z, crypt_str("rot_z"), misc);

			LoadBool(&vars.visuals.editparticles, crypt_str("editparticles"), misc);
			LoadFloat(&vars.visuals.smoke_alpha, crypt_str("smoke_alpha"), misc);
			LoadColor(&vars.visuals.molotov_particle_color, crypt_str("molotov_particle_color"), misc);
			LoadColor(&vars.visuals.blood_particle_color, crypt_str("blood_particle_color"), misc);


			auto& fog = misc[crypt_str("fog")]; {
				LoadBool(&vars.visuals.fog.enabled, crypt_str("enabled"), fog);
				LoadFloat(&vars.visuals.fog.start, crypt_str("start"), fog);
				LoadFloat(&vars.visuals.fog.end, crypt_str("end"), fog);
				LoadFloat(&vars.visuals.fog.density, crypt_str("density"), fog);
				LoadColor(&vars.visuals.fog.color, crypt_str("color"), fog);
			}

			auto& motionblur = misc[crypt_str("motionblur")];
			{
				LoadBool(&vars.visuals.motionblur.enabled, crypt_str("enabled"), motionblur);
				LoadBool(&vars.visuals.motionblur.fwd_enabled, crypt_str("fwd_enabled"), motionblur);
				LoadFloat(&vars.visuals.motionblur.falling_min, crypt_str("falling_min"), motionblur);
				LoadFloat(&vars.visuals.motionblur.falling_max, crypt_str("falling_max"), motionblur);
				LoadFloat(&vars.visuals.motionblur.falling_intensity, crypt_str("falling_intensity"), motionblur);
				LoadFloat(&vars.visuals.motionblur.rotation_intensity, crypt_str("rotation_intensity"), motionblur);
				LoadFloat(&vars.visuals.motionblur.strength, crypt_str("strength"), motionblur);
			}
		}
	}

	auto& world = json[crypt_str("world")]; {
		auto& weapons = world[crypt_str("weapons")]; {
			LoadBool(&vars.visuals.world.weapons.enabled, crypt_str("enable"), weapons);
			LoadBool(&vars.visuals.world.weapons.planted_bomb, crypt_str("planted_bomb"), weapons);
			LoadFloat(&vars.visuals.world.weapons.planted_bombheight, crypt_str("planted_bombheight"), weapons);
			LoadColor(&vars.visuals.world.weapons.color, crypt_str("color"), weapons);
			LoadBool(&vars.visuals.world.weapons.indtype[0], crypt_str("indtype0"), weapons);
			LoadBool(&vars.visuals.world.weapons.indtype[1], crypt_str("indtype1"), weapons);
		}
		auto& projectiles = world[crypt_str("projectiles")]; {
			LoadBool(&vars.visuals.world.projectiles.enable, crypt_str("enable"), projectiles);
			LoadBool(&vars.visuals.world.projectiles.filter[0], crypt_str("filter0"), projectiles);
			LoadBool(&vars.visuals.world.projectiles.filter[1], crypt_str("filter1"), projectiles);
			LoadBool(&vars.visuals.world.projectiles.filter[2], crypt_str("filter2"), projectiles);
			LoadBool(&vars.visuals.world.projectiles.indtype[0], crypt_str("indtype0"), projectiles);
			LoadBool(&vars.visuals.world.projectiles.indtype[1], crypt_str("indtype1"), projectiles);
#ifdef _DEBUG
			LoadBool(&vars.visuals.world.projectiles.trajectories, crypt_str("trajectories"), projectiles);
			LoadColor(&vars.visuals.world.projectiles.colors[0], crypt_str("trajectory_team"), projectiles);
			LoadColor(&vars.visuals.world.projectiles.colors[1], crypt_str("trajectory_enemy"), projectiles);
			LoadColor(&vars.visuals.world.projectiles.colors[2], crypt_str("trajectory_local"), projectiles);
#endif
		}
		auto& tracers = world[crypt_str("tracers")]; {
			LoadBool(&vars.visuals.bullet_tracer, crypt_str("enable"), tracers[crypt_str("bullet_tracer")]);
			LoadFloat(&vars.visuals.impacts_size, crypt_str("size"), tracers[crypt_str("bullet_tracer")]);
			LoadInt(&vars.visuals.bullet_tracer_type, crypt_str("sprite"), tracers[crypt_str("bullet_tracer")]);
			LoadColor(&vars.visuals.bullet_tracer_color, crypt_str("color"), tracers[crypt_str("bullet_tracer")]);
			LoadBool(&vars.visuals.bullet_tracer_local, crypt_str("local"), tracers[crypt_str("bullet_tracer")]);
			LoadColor(&vars.visuals.bullet_tracer_local_color, crypt_str("local_color"), tracers[crypt_str("bullet_tracer")]);

			LoadBool(&vars.visuals.bullet_impact, crypt_str("enable"), tracers[crypt_str("bullet_impact")]);
			LoadColor(&vars.visuals.bullet_impact_color, crypt_str("color"), tracers[crypt_str("bullet_impact")]);
		}
		auto& effects = world[crypt_str("effects")]; {
			for (int i = 0; i < 8; i++)
				LoadBool(&vars.visuals.remove[i], std::string(crypt_str("remove") + to_string(i)).c_str(), effects);
			LoadBool(&vars.visuals.force_crosshair, crypt_str("force_crosshair"), effects);
			LoadBool(&vars.visuals.kill_effect, crypt_str("kill_effect"), effects);
			LoadInt(&vars.misc.worldfov, crypt_str("world_fov"), effects);
			LoadInt(&vars.misc.viewmodelfov, crypt_str("viewmodel_fov"), effects);
			LoadBool(&vars.visuals.recoil_crosshair, crypt_str("recoil_crosshair"), effects);
			LoadBool(&vars.visuals.radarhack, crypt_str("radarhack"), effects);

			LoadBool(&vars.visuals.nightmode, crypt_str("enable"), effects[crypt_str("nightmode")]);
			LoadBool(&vars.visuals.customize_color, crypt_str("customize_color"), effects[crypt_str("nightmode")]);
			LoadInt(&vars.visuals.nightmode_amount, crypt_str("nightmode_amount"), effects[crypt_str("nightmode")]);
			LoadColor(&vars.visuals.nightmode_color, crypt_str("world"), effects[crypt_str("nightmode")][crypt_str("color")]);
			LoadColor(&vars.visuals.nightmode_prop_color, crypt_str("prop"), effects[crypt_str("nightmode")][crypt_str("color")]);
			LoadColor(&vars.visuals.nightmode_skybox_color, crypt_str("skybox"), effects[crypt_str("nightmode")][crypt_str("color")]);
		}

		auto& chicken = world[crypt_str("chicken")];{
			LoadBool(&vars.misc.chicken.enable, crypt_str("enable"), chicken);
			LoadBool(&vars.misc.chicken.chickenbox, crypt_str("chickenbox"), chicken);
			LoadBool(&vars.misc.chicken.owner, crypt_str("owner"), chicken);
			LoadBool(&vars.misc.chicken.revealkiller, crypt_str("revealkiller"), chicken);
			LoadBool(&vars.misc.chicken.saykiller, crypt_str("saykiller"), chicken);
			LoadBool(&vars.misc.chicken.sidelist, crypt_str("sidelist"), chicken);
			LoadBool(&vars.misc.chicken.fish, crypt_str("fish"), chicken);
			LoadBool(&vars.misc.chicken.fishbox, crypt_str("fishbox"), chicken);
			LoadColor(&vars.misc.chicken.fishcolor, crypt_str("fishcolor"), chicken);
		}
	}

	auto& misc = json[crypt_str("misc")]; {
		LoadBool(&vars.misc.antiuntrusted, crypt_str("anti_untrusted"), misc);
		LoadBool(&vars.misc.ublockOrigin, crypt_str("ublockorigin"), misc);
		LoadBool(&vars.misc.unlockinventoryaccess, crypt_str("unlockinventoryaccess"), misc);
		LoadBool(&vars.misc.bunnyhop, crypt_str("bunnyhop"), misc);
		LoadBool(&vars.misc.autostrafe, crypt_str("autostrafe"), misc);
		LoadBool(&vars.misc.autostrafeonkey, crypt_str("autostrafeonkey"), misc);
		LoadBind(&vars.misc.autostrafekey, crypt_str("autostrafekey"), misc);
		LoadBool(&vars.misc.knifebot, crypt_str("knifebot"), misc);
		LoadInt(&vars.misc.restrict_type, crypt_str("restrict_type"), misc);
		LoadBool(&vars.visuals.clantagspammer, crypt_str("clantag"), misc);
		LoadInt(&vars.visuals.clantagtype, crypt_str("clantagtype"), misc);
		std::string clantagpro;
		LoadStr(&clantagpro, crypt_str("clantagcustomname"), misc);
		strcpy_s(vars.visuals.clantagcustomname, 32, clantagpro.c_str());
		LoadBool(&vars.visuals.clantagrotating, crypt_str("clantagrotating"), misc);
		LoadFloat(&vars.visuals.clantagspeed, crypt_str("clantagspeed"), misc);
		LoadBool(&vars.misc.revealranks, crypt_str("revealranks"), misc);
		LoadBool(&vars.misc.autobuy.enable, crypt_str("enable"), misc[crypt_str("autobuy")]);
		LoadInt(&vars.misc.autobuy.pistol, crypt_str("pistol"), misc[crypt_str("autobuy")]);
		LoadInt(&vars.misc.autobuy.main, crypt_str("main"), misc[crypt_str("autobuy")]);
		for (int i = 0; i < 7; i++)
			LoadBool(&vars.misc.autobuy.misc[i], std::string(crypt_str("autobuy_misc") + to_string(i)).c_str(), misc);
		LoadInt(&vars.visuals.playermodelct, crypt_str("playermodelct"), misc);
		LoadInt(&vars.visuals.playermodelt, crypt_str("playermodelt"), misc);
		
		LoadBool(&vars.misc.enableviewmodel, crypt_str("enable"), misc[crypt_str("viewmodel")]);
		LoadFloat(&vars.misc.viewmodel_x, crypt_str("x"), misc[crypt_str("viewmodel")]);
		LoadFloat(&vars.misc.viewmodel_y, crypt_str("y"), misc[crypt_str("viewmodel")]);
		LoadFloat(&vars.misc.viewmodel_z, crypt_str("z"), misc[crypt_str("viewmodel")]);
		LoadFloat(&vars.misc.viewmodel_roll, crypt_str("roll"), misc[crypt_str("viewmodel")]);
		LoadInt(&vars.menu.menu_key, crypt_str("menu_key"), misc);
		LoadInt(&vars.menu.window_opacity, crypt_str("window_opacity"), misc);

		int lua_count = 0;
		CLua::Get().refresh_scripts();
		LoadInt(&lua_count, crypt_str("loaded_luas_count"), misc);
		for (int i = 0; i < lua_count; i++)
		{
			std::string curluaname;
			LoadStr(&curluaname, std::string(crypt_str("loaded_luas_") + to_string(i)).c_str(), misc);
			CLua::Get().load_script(CLua::Get().get_script_id(curluaname));
		}
	}

	auto& movement = json[crypt_str("movement")];{
		LoadBool(&vars.movement.edgebug.enabled, crypt_str("enabled"), movement[crypt_str("edgebug")]);
		LoadBind(&vars.movement.edgebug.key, crypt_str("edgebug-key"), movement);
		LoadInt(&vars.movement.edgebug.ticks, crypt_str("ticks"), movement[crypt_str("edgebug")]);
		LoadFloat(&vars.movement.edgebug.lock, crypt_str("lock"), movement[crypt_str("edgebug")]);
		LoadInt(&vars.movement.edgebug.locktype, crypt_str("locktype"), movement[crypt_str("edgebug")]);
		LoadBool(&vars.movement.edgebug.marker, crypt_str("marker"), movement[crypt_str("edgebug")]);
		LoadBool(&vars.movement.edgebug.detect_strafe, crypt_str("detect_strafe"), movement[crypt_str("edgebug")]);
		LoadColor(&vars.movement.edgebug.marker_color, crypt_str("marker_color"), movement[crypt_str("edgebug")]);
		LoadInt(&vars.movement.edgebug.marker_thickness, crypt_str("marker_thickness"), movement[crypt_str("edgebug")]);
		LoadBool(&vars.movement.jumpbug, crypt_str("jumpbug"), movement);
		LoadBind(&vars.movement.jumpbugkey, crypt_str("jumpbug-key"), movement);
		LoadInt(&vars.movement.jumpbugtype, crypt_str("jumpbugtype"), movement);
		LoadFloat(&vars.movement.lengthcheck, crypt_str("lengthcheck"), movement);
		LoadBool(&vars.movement.edgejump, crypt_str("edgejump"), movement);
		LoadBind(&vars.movement.edgejumpkey, crypt_str("edgejump-key"), movement);
		LoadBool(&vars.movement.ladderej, crypt_str("edgejump-ladder"), movement);
		LoadBool(&vars.movement.longjump, crypt_str("longjump"), movement);
		LoadBind(&vars.movement.longjumpkey, crypt_str("longjump-key"), movement);
		LoadBool(&vars.movement.minijump, crypt_str("minijump"), movement);
		LoadBind(&vars.movement.minijumpkey, crypt_str("minijump-key"), movement);
		LoadBool(&vars.movement.showvelocity, crypt_str("showvelocity"), movement);
		LoadBool(&vars.movement.showtakeoff, crypt_str("showtakeoff"), movement);
		LoadFloat(&vars.movement.showvelpos, crypt_str("showvelpos"), movement);
		LoadBool(&vars.movement.staticvelcolor, crypt_str("staticvelcolor"), movement);
		LoadColor(&vars.movement.velcolor, crypt_str("velcolor"), movement);
		LoadBool(&vars.movement.velocityfade, crypt_str("velocityfade"), movement);
		LoadFloat(&vars.movement.velfadevelocity, crypt_str("velfadevelocity"), movement);
		LoadInt(&vars.movement.showveltype, crypt_str("showveltype"), movement);
		LoadBool(&vars.movement.ljstats, crypt_str("ljstats"), movement);
		LoadBool(&vars.movement.ebdetect.mastertoggle, crypt_str("ebdetectmastertoggle"), movement);
		LoadBool(&vars.movement.ebdetect.enabled[0], crypt_str("ebdetecteffect"), movement);
		LoadBool(&vars.movement.ebdetect.enabled[1], crypt_str("ebdetectchat"), movement);
		LoadBool(&vars.movement.ebdetect.enabled[2], crypt_str("ebdetectcounter"), movement);
		LoadInt(&vars.movement.ebdetect.soundtype, crypt_str("ebdetectsoundtype"), movement);
		LoadInt(&vars.movement.autostrafetype, crypt_str("autostrafetype"), movement);
		LoadInt(&vars.movement.dxtostrafe, crypt_str("dxtostrafe"), movement);
		LoadBool(&vars.movement.msl, crypt_str("msl"), movement);
		LoadFloat(&vars.movement.mslx, crypt_str("mslx"), movement);
		LoadBool(&vars.movement.ebdetect.enabled[3], crypt_str("ebdetectsound"), movement);
		std::string cc;
		LoadStr(&cc, crypt_str("ebdetectsoundname"), movement);
		strcpy_s(vars.movement.ebdetect.sound_name, 32, cc.c_str());
		LoadBool(&vars.movement.fastduck, crypt_str("fastduck"), movement);
		LoadBool(&vars.movement.slidewalk, crypt_str("slidewalk"), movement);
		LoadBool(&vars.movement.autoduck, crypt_str("autoduck"), movement);
		LoadFloat(&vars.movement.autoduckbefore, crypt_str("autoduckbefore"), movement);
		LoadFloat(&vars.movement.autoduckafter, crypt_str("autoduckafter"), movement);
		LoadBool(&vars.movement.ladderglide, crypt_str("ladderglide"), movement);
		LoadBind(&vars.movement.ladderglidekey, crypt_str("ladderglidekey"), movement);
		LoadBool(&vars.movement.autopixelsurf, crypt_str("autopixelsurf"), movement);
		LoadBind(&vars.movement.autopixelsurfkey, crypt_str("autopixelsurfkey"), movement);
		LoadInt(&vars.movement.autopixelsurf_ticks, crypt_str("autopixelsurf_ticks"), movement);
		LoadBind(&vars.movement.autopixelsurf_align, crypt_str("autopixelsurf_aligner"), movement);
		LoadBool(&vars.movement.autopixelsurf_freestand, crypt_str("autopixelsurf_freestand"), movement);
		LoadBool(&vars.movement.autopixelsurf_alignind, crypt_str("autopixelsurf_alignind"), movement);
		LoadColor(&vars.movement.autopixelsurf_indcol1, crypt_str("autopixelsurf_indcol1"), movement);

		LoadBool(&vars.movement.showstamina, crypt_str("showstamina"), movement);
		LoadInt(&vars.movement.showstaminatype, crypt_str("showstaminatype"), movement);
		LoadFloat(&vars.movement.staminascrpos, crypt_str("staminascrpos"), movement);
		LoadBool(&vars.movement.staminatakeoff, crypt_str("staminatakeoff"), movement);
		LoadColor(&vars.movement.staminacol, crypt_str("staminacol"), movement);
		LoadBool(&vars.movement.staminafade, crypt_str("staminafade"), movement);
		LoadFloat(&vars.movement.staminafadevel, crypt_str("staminafadevel"), movement);

		auto& graph = movement[crypt_str("graph")]; {
			LoadBool(&vars.movement.graph.enabled, crypt_str("enabled"), graph);
			LoadBool(&vars.movement.graph.types[0], crypt_str("types0"), graph);
			LoadBool(&vars.movement.graph.types[1], crypt_str("types1"), graph);
			LoadColor(&vars.movement.graph.staminacol, crypt_str("staminacol"), graph);
			LoadColor(&vars.movement.graph.staminatextcol, crypt_str("staminatextcol"), graph);
			LoadFloat(&vars.movement.graph.staminatofade, crypt_str("staminatofade"), graph);
			LoadBool(&vars.movement.graph.staminagaincolors, crypt_str("staminagaincolors"), graph);

			LoadColor(&vars.movement.graph.color, crypt_str("color"), graph);
			LoadColor(&vars.movement.graph.textcolor, crypt_str("textcolor"), graph);
			LoadFloat(&vars.movement.graph.height, crypt_str("height"), graph);
			LoadInt(&vars.movement.graph.linewidth, crypt_str("linewidth"), graph);
			LoadInt(&vars.movement.graph.size, crypt_str("size"), graph);
			LoadInt(&vars.movement.graph.thickness, crypt_str("thickness"), graph);
			LoadFloat(&vars.movement.graph.speed, crypt_str("speed"), graph);
			LoadInt(&vars.movement.graph.fadetype, crypt_str("fadetype"), graph);
			LoadFloat(&vars.movement.graph.speedtofade, crypt_str("speedtofade"), graph);
			LoadFloat(&vars.movement.graph.xscreenpos, crypt_str("xscreenpos"), graph);
			LoadBool(&vars.movement.graph.gaincolors, crypt_str("gaincolors"), graph);
		}

		auto& recorder = movement[crypt_str("recorder")]; {
			LoadBool(&vars.movement.recorder.enabled, crypt_str("enabled"), recorder);
			LoadBool(&vars.movement.recorder.infowindow, crypt_str("infowindow"), recorder);
			for (int i = 0; i < vars.movement.recorder.infowindowselected.size(); i++)
			{
				auto& curjson = recorder[crypt_str("infowindowselected")][i];
				LoadBool(&vars.movement.recorder.infowindowselected.at(i), crypt_str("comp"), curjson);
			}
			LoadFloat(&vars.movement.recorder.smoothing, crypt_str("smoothing"), recorder);
			LoadInt(&vars.movement.recorder.maxrender, crypt_str("maxrender"), recorder);
			LoadBind(&vars.movement.recorder.clearrecord, crypt_str("clearrecord"), recorder);
			LoadBind(&vars.movement.recorder.startplayback, crypt_str("startplayback"), recorder);
			LoadBind(&vars.movement.recorder.stopplayback, crypt_str("stopplayback"), recorder);
			LoadBind(&vars.movement.recorder.startrecord, crypt_str("startrecord"), recorder);
			LoadBind(&vars.movement.recorder.stoprecord, crypt_str("stoprecord"), recorder);
			LoadBool(&vars.movement.recorder.lockva, crypt_str("lockva"), recorder);
			LoadFloat(&vars.movement.recorder.ringsize, crypt_str("ringsize"), recorder);
			LoadColor(&vars.movement.recorder.startring, crypt_str("startring"), recorder);
			LoadColor(&vars.movement.recorder.endring, crypt_str("endring"), recorder);
			LoadColor(&vars.movement.recorder.linecolor, crypt_str("linecolor"), recorder);
			LoadColor(&vars.movement.recorder.textcolor, crypt_str("textcolor"), recorder);
		}

		auto& checkpoint = movement[crypt_str("checkpoint")];
		{
			LoadBool(&vars.movement.checkpoint.enabled, crypt_str("enabled"), checkpoint);
			LoadBind(&vars.movement.checkpoint.teleportkey, crypt_str("teleportkey"), checkpoint);
			LoadBind(&vars.movement.checkpoint.checkpointkey, crypt_str("checkpointkey"), checkpoint);
			LoadBind(&vars.movement.checkpoint.nextkey, crypt_str("nextkey"), checkpoint);
			LoadBind(&vars.movement.checkpoint.prevkey, crypt_str("prevkey"), checkpoint);
			LoadBind(&vars.movement.checkpoint.undokey, crypt_str("undokey"), checkpoint);
			LoadColor(&vars.movement.checkpoint.color, crypt_str("color"), checkpoint);
		}

		auto& indicators = movement[crypt_str("indicators")];
		{
			LoadBool(&vars.movement.indicators.show_active, crypt_str("show_active"), indicators);
			LoadFloat(&vars.movement.indicators.height, crypt_str("height"), indicators);
			LoadColor(&vars.movement.indicators.active_col, crypt_str("active_col"), indicators);
			LoadColor(&vars.movement.indicators.passive_col, crypt_str("passive_col"), indicators);
			for (int i = 0; i < 8; i++)
			{
				LoadBool(&vars.movement.indicators.on[i], crypt_str("ind"), indicators[crypt_str("on")][i]);
			}

		}

	}

	auto& skinchanger = json[crypt_str("skinchanger")];
	{
		size_t size = 0; LoadUInt(&size, crypt_str("itemssize"), skinchanger);
		if (size == game_data::weapon_names.size())
		{
			for (size_t i = 0; i < size; i++)
			{
				auto& skinchangerweapon = skinchanger[crypt_str("skinweapon")][i];
				item_setting to_push{};
				std::string nm;
				LoadStr(&nm, crypt_str("custom_name"), skinchangerweapon);
				nm.resize(32);
				strcpy_s(to_push.custom_name, nm.c_str());
				LoadInt(&to_push.definition_index, crypt_str("definition_index"), skinchangerweapon);
				LoadInt(&to_push.definition_override_index, crypt_str("definition_override_index"), skinchangerweapon);
				LoadInt(&to_push.definition_override_vector_index, crypt_str("definition_override_vector_index"), skinchangerweapon);
				LoadInt(&to_push.definition_vector_index, crypt_str("definition_vector_index"), skinchangerweapon);
				LoadBool(&to_push.enabled, crypt_str("enabled"), skinchangerweapon);
				LoadInt(&to_push.entity_quality_index, crypt_str("entity_quality_index"), skinchangerweapon);
				LoadInt(&to_push.entity_quality_vector_index, crypt_str("entity_quality_vector_index"), skinchangerweapon);

				std::string nm2;
				LoadStr(&nm2, crypt_str("name"), skinchangerweapon);
				nm2.resize(32);
				strcpy_s(to_push.name, nm2.c_str());
				LoadInt(&to_push.paint_kit_index, crypt_str("paint_kit_index"), skinchangerweapon);
				LoadInt(&to_push.paint_kit_vector_index, crypt_str("paint_kit_vector_index"), skinchangerweapon);
				LoadInt(&to_push.seed, crypt_str("seed"), skinchangerweapon);
				LoadInt(&to_push.stat_trak, crypt_str("stat_trak"), skinchangerweapon);
				LoadInt(&to_push.colors[0], crypt_str("colors0"), skinchangerweapon);
				LoadInt(&to_push.colors[1], crypt_str("colors1"), skinchangerweapon);
				LoadInt(&to_push.colors[2], crypt_str("colors2"), skinchangerweapon);
				LoadInt(&to_push.colors[3], crypt_str("colors3"), skinchangerweapon);

				game_data::skin_kits[to_push.paint_kit_vector_index].link->color1 = to_push.colors[0];
				game_data::skin_kits[to_push.paint_kit_vector_index].link->color2 = to_push.colors[1];
				game_data::skin_kits[to_push.paint_kit_vector_index].link->color3 = to_push.colors[2];
				game_data::skin_kits[to_push.paint_kit_vector_index].link->color4 = to_push.colors[3];

				LoadFloat(&to_push.pearlescent, crypt_str("pearlescent"), skinchangerweapon);
				game_data::skin_kits[to_push.paint_kit_vector_index].link->pearlescent = to_push.pearlescent;

				LoadFloat(&to_push.wear, crypt_str("wear"), skinchangerweapon);
				g_config.get_items().at(i) = to_push;
			}
		}
	}

	for (size_t i = 0; i < vars.chams.size(); i++) {
		const auto& chamsJson = json[crypt_str("Chams")][i];
		auto& chamsConfig = vars.chams[i];

		for (size_t j = 0; j < chamsConfig.materials.size(); j++) {
			const auto& materialsJson = chamsJson[j];
			auto& materialsConfig = vars.chams[i].materials[j];

			if (materialsJson.isMember(crypt_str("Enabled"))) materialsConfig.enabled = materialsJson[crypt_str("Enabled")].asBool();
			if (materialsJson.isMember(crypt_str("Health based"))) materialsConfig.healthBased = materialsJson[crypt_str("Health based")].asBool();
			if (materialsJson.isMember(crypt_str("Blinking"))) materialsConfig.blinking = materialsJson[crypt_str("Blinking")].asBool();
			if (materialsJson.isMember(crypt_str("Material"))) materialsConfig.material = materialsJson[crypt_str("Material")].asInt();
			if (materialsJson.isMember(crypt_str("Wireframe"))) materialsConfig.wireframe = materialsJson[crypt_str("Wireframe")].asBool();
			if (materialsJson.isMember(crypt_str("Color"))) {
				const auto& colorJson = materialsJson[crypt_str("Color")];
				auto& colorConfig = materialsConfig; // leftover

				if (colorJson.isMember(crypt_str("Color"))) {
					colorConfig.color[0] = colorJson[crypt_str("Color")][0].asFloat();
					colorConfig.color[1] = colorJson[crypt_str("Color")][1].asFloat();
					colorConfig.color[2] = colorJson[crypt_str("Color")][2].asFloat();

					if (colorJson[crypt_str("Color")].size() == 4)
						colorConfig.color[3] = colorJson[crypt_str("Color")][3].asFloat();
				}

				if (colorJson.isMember(crypt_str("Rainbow"))) colorConfig.rainbow = colorJson["Rainbow"].asBool();
				if (colorJson.isMember(crypt_str("Rainbow speed"))) colorConfig.rainbowSpeed = colorJson[crypt_str("Rainbow speed")].asFloat();
			}
			if (materialsJson.isMember(crypt_str("Alpha"))) materialsConfig.color[3] = materialsJson[crypt_str("Alpha")].asFloat();
		}
	}


	Msg(std::string(crypt_str("Loaded Config ") + cfg_name), vars.visuals.eventlog_color);
	vars.menu.inLoad = true;
	vars.menu.inLoad2 = true;
	vars.menu.inLoad3 = true;
	vars.menu.inLoadteamdmg = true;
	vars.menu.inLoadTeleport = true;
}

CConfig Config;
CGlobalVariables vars;