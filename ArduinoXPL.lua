-- konfigurace ==========================
AXPL_DEBUG = 1
AXPL_GPS = 1
-- ======================================

print('ArduinoXPL')
print('----------------')
str = "Debug: "
if(AXPL_DEBUG == 1) then str = str .. 'ON' else str = str .. 'OFF' end
print(str)
str = "GPS: "
if(AXPL_GPS == 1) then str = str .. 'ON' else str = str .. 'OFF' end
print(str)
print('----------------')

-- ArduinoXPL - COM
lmc_add_com('COM', 'COM4', 9600, 8, 'N', 1)
lmc_set_com_splitter('COM', '\n')  

-- zpracovani prijatych dat z COM
lmc_set_handler('COM', function(comVal)
  if(AXPL_DEBUG == 1) then
    print("Arduino > LUA: " .. comVal)
  end

  if(string.sub(comVal, 1, 4) == "AD1_") then
    val = string.sub(comVal,5)
    lmc_set_xpl_variable('sim/cockpit2/radios/actuators/adf1_frequency_hz', val + 0)
  end

  if(string.sub(comVal, 1, 4) == "AD2_") then
    val = string.sub(comVal,5)
    lmc_set_xpl_variable('sim/cockpit2/radios/actuators/adf2_frequency_hz', val + 0)
  end

  if(string.sub(comVal, 1, 4) == "HDG_") then
    val = string.sub(comVal,5)
    lmc_set_xpl_variable('sim/cockpit2/autopilot/heading_dial_deg_mag_pilot', val + 0)
  end

  if(string.sub(comVal, 1, 4) == "CRS_") then
    val = string.sub(comVal,5)
    lmc_set_xpl_variable('sim/cockpit2/radios/actuators/hsi_obs_deg_mag_pilot', val + 0)
  end

  if(string.sub(comVal, 1, 4) == "NA1_") then
    val = string.sub(comVal,5)
    lmc_set_xpl_variable('sim/cockpit2/radios/actuators/nav1_frequency_hz', val + 0)
  end

  if(string.sub(comVal, 1, 4) == "NA2_") then
    val = string.sub(comVal,5)
    lmc_set_xpl_variable('sim/cockpit2/radios/actuators/nav2_frequency_hz', val + 0)
  end

  if(string.sub(comVal, 1, 4) == "CO1_") then
    val = string.sub(comVal,5)
    lmc_set_xpl_variable('sim/cockpit2/radios/actuators/com1_frequency_hz', val + 0)
  end

  if(string.sub(comVal, 1, 4) == "CO2_") then
    val = string.sub(comVal,5)
    lmc_set_xpl_variable('sim/cockpit2/radios/actuators/com2_frequency_hz', val + 0)
  end

  if(string.sub(comVal, 1, 3) == "GP1") then
    lmc_xpl_command('sim/GPS/g430n1_direct')
  end

  if(string.sub(comVal, 1, 3) == "GP2") then
    lmc_xpl_command('sim/GPS/g430n1_ent')
  end

  if(string.sub(comVal, 1, 3) == "GP3") then
    lmc_xpl_command('sim/GPS/g430n1_chapter_up')
  end

  if(string.sub(comVal, 1, 3) == "GP4") then
    lmc_xpl_command('sim/GPS/g430n1_chapter_dn')
  end

  if(string.sub(comVal, 1, 3) == "GP5") then
    lmc_xpl_command('sim/GPS/g430n1_page_up')
  end

  if(string.sub(comVal, 1, 3) == "GP6") then
    lmc_xpl_command('sim/GPS/g430n1_page_dn')
  end

  if(string.sub(comVal, 1, 3) == "APM") then
    lmc_xpl_command('sim/autopilot/servos_toggle')
    if(AXPL_DEBUG == 1) then
      print("LUA > XPL: sim/autopilot/servos_toggle")
    end
  end

  if(string.sub(comVal, 1, 3) == "APH") then
    lmc_xpl_command('sim/autopilot/heading')
    if(AXPL_DEBUG == 1) then
      print("LUA > XPL: sim/autopilot/heading")
    end
  end  

  if(string.sub(comVal, 1, 3) == "APN") then
    lmc_xpl_command('sim/autopilot/NAV')
    if(AXPL_DEBUG == 1) then
      print("LUA > XPL: sim/autopilot/NAV")
    end
  end
  
  if(string.sub(comVal, 1, 3) == "APA") then
    lmc_xpl_command('sim/autopilot/altitude_hold')
    if(AXPL_DEBUG == 1) then
      print("LUA > XPL: sim/autopilot/altitude_hold")
    end
  end  
  
end)

-- GPS - jen pokud je zapnuta
if(AXPL_GPS == 1) then

  -- GPS to
	lmc_on_xpl_var_change('sim/cockpit2/radios/indicators/gps_nav_id', function(value)
	  str = 'GP1_' .. value .. '|'
	  lmc_send_to_com('COM', str)
	  if(AXPL_DEBUG == 1) then
		print("LUA > Arduino: " .. str .. " (GPS to)")
	  end
	end)

	-- GPS distance NM
	lmc_on_xpl_var_change('sim/cockpit2/radios/indicators/gps_dme_distance_nm', function(value, count)
    if(type(value) == "number") then
      str = 'GP2_' .. string.format("%.1f", value) .. '|'
      lmc_send_to_com('COM', str)
      if(AXPL_DEBUG == 1) then
        print("LUA > Arduino: " .. str .. " (GPS distance)")
      end
	  end
	end, 5000)

	-- GPS DTK
	lmc_on_xpl_var_change('sim/cockpit/radios/gps_course_degtm', function(value, count)
	  str = 'GP3_' .. string.format("%d", value) .. '|'
	  lmc_send_to_com('COM', str)
	  if(AXPL_DEBUG == 1) then
		print("LUA > Arduino: " .. str .. " (GPS DTK)")
	  end
	end, 1000, 1)

	-- GPS ETE sec
	lmc_on_xpl_var_change('sim/cockpit/radios/gps_dme_time_secs', function(value, count)
	  if(value == math.huge) then value = 0 end 
	  str = 'GP4_' .. string.format("%d", value * 60) .. '|'
	  lmc_send_to_com('COM', str)
	  if(AXPL_DEBUG == 1) then
		  print("LUA > Arduino: " .. str .. " (GPS ETE)")
	  end
	end, 1000, 1)	

  -- GPS dme speed kts
	lmc_on_xpl_var_change('sim/cockpit/radios/gps_dme_speed_kts', function(value, count)
	  str = 'GP5_' .. string.format("%d", value) .. '|'
	  lmc_send_to_com('COM', str)
	  if(AXPL_DEBUG == 1) then
		print("LUA > Arduino: " .. str .. " (GPS speed)")
	  end
	end, 1000, 1)

	-- GPS bearing
	lmc_on_xpl_var_change('sim/cockpit2/radios/indicators/gps_bearing_deg_mag', function(value, count)
	  str = 'GP6_' .. string.format("%d", value) .. '|'
	  lmc_send_to_com('COM', str)
	  if(AXPL_DEBUG == 1) then
		print("LUA > Arduino: " .. str .. " (GPS bearing)")
	  end
	end, 1000, 1)

	-- GPS TRK (spatne - neni to GPS TRK, je to natoceni era...)
	lmc_on_xpl_var_change('sim/flightmodel/position/magpsi', function(value, count)
	  str = 'GP7_' .. string.format("%d", value) .. '|'
	  lmc_send_to_com('COM', str)
	  if(AXPL_DEBUG == 1) then
		print("LUA > Arduino: " .. str .. " (GPS TRK)")
	  end
	end, 1000, 1)	

end

-- Avionics on
lmc_on_xpl_var_change('sim/cockpit/electrical/avionics_on', function(value)
  str = 'AVO_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  if(AXPL_DEBUG == 1) then
    print("LUA > Arduino: " .. str)
  end
end)

-- Low voltage
lmc_on_xpl_var_change('sim/cockpit/warnings/annunciators/low_voltage', function(value)
  str = 'LOV_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  if(AXPL_DEBUG == 1) then
    print("LUA > Arduino: " .. str)
  end
end)

-- Autopilot mode
lmc_on_xpl_var_change('sim/cockpit/autopilot/autopilot_mode', function(value)
  str = 'APM_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  if(AXPL_DEBUG == 1) then
    print("LUA > Arduino: " .. str)
  end
end)

-- Autopilot - heading mode
lmc_on_xpl_var_change('sim/cockpit2/autopilot/heading_mode', function(value)
  str = 'APH_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  if(AXPL_DEBUG == 1) then
    print("LUA > Arduino: " .. str)
  end
end)

-- Autopilot - hnav armed
lmc_on_xpl_var_change('sim/cockpit2/autopilot/hnav_armed', function(value)
  str = 'APN_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  if(AXPL_DEBUG == 1) then
    print("LUA > Arduino: " .. str)
  end
end)

-- Autopilot - alt hold armed
lmc_on_xpl_var_change('sim/cockpit2/autopilot/altitude_hold_armed', function(value)
  str = 'APA_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  if(AXPL_DEBUG == 1) then
    print("LUA > Arduino: " .. str)
  end
end)

-- Autopilot - alt mode
lmc_on_xpl_var_change('sim/cockpit2/autopilot/altitude_mode', function(value)
  str = 'APB_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  if(AXPL_DEBUG == 1) then
    print("LUA > Arduino: " .. str)
  end
end)

os.execute("timeout " .. tonumber(5))   