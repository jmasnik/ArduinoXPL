print('ArduinoXPL')

-- ArduinoXPL - COM
lmc_add_com('COM', 'COM4', 9600, 8, 'N', 1)
lmc_set_com_splitter('COM', '\n')  

-- zpracovani prijatych dat z COM
lmc_set_handler('COM', function(comVal)
  --print(">>" .. comVal .. "<<")

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
  end

end)

-- GPS to
lmc_on_xpl_var_change('sim/cockpit2/radios/indicators/gps_nav_id', function(value)
  lmc_send_to_com('COM', 'GP1_' .. value .. '|')
end)

-- GPS distance NM
lmc_on_xpl_var_change('sim/cockpit2/radios/indicators/gps_dme_distance_nm', function(value, count)
  lmc_send_to_com('COM', 'GP2_' .. string.format("%.1f", value) .. '|')
end, 2000)

-- GPS DTK
lmc_on_xpl_var_change('sim/cockpit/radios/gps_course_degtm', function(value, count)
  lmc_send_to_com('COM', 'GP3_' .. string.format("%d", value) .. '|')
end, 2000)

-- GPS ETE sec
lmc_on_xpl_var_change('sim/cockpit/radios/gps_dme_time_secs', function(value, count)
  lmc_send_to_com('COM', 'GP4_' .. string.format("%d", value * 60) .. '|')
end, 2000)

-- GPS dme speed kts
lmc_on_xpl_var_change('sim/cockpit/radios/gps_dme_speed_kts', function(value, count)
  lmc_send_to_com('COM', 'GP5_' .. string.format("%d", value) .. '|')
end, 2000)

-- GPS bearing
lmc_on_xpl_var_change('sim/cockpit2/radios/indicators/gps_bearing_deg_mag', function(value, count)
  lmc_send_to_com('COM', 'GP6_' .. string.format("%d", value) .. '|')
end, 2000)

-- GPS TRK (spatne - neni to GPS TRK, je to natoceni era...)
lmc_on_xpl_var_change('sim/flightmodel/position/magpsi', function(value, count)
  lmc_send_to_com('COM', 'GP7_' .. string.format("%d", value) .. '|')
end, 2000)

-- Avionics on
lmc_on_xpl_var_change('sim/cockpit/electrical/avionics_on', function(value)
  str = 'AVO_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  print(str)
end)

-- Low voltage
lmc_on_xpl_var_change('sim/cockpit/warnings/annunciators/low_voltage', function(value)
  str = 'LOV_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  print(str)
end)

-- Autopilot mode
lmc_on_xpl_var_change('sim/cockpit/autopilot/autopilot_mode', function(value)
  str = 'APM_' .. string.format("%d", value) .. '|'
  lmc_send_to_com('COM', str)
  print(str)
end)

os.execute("timeout " .. tonumber(6))   