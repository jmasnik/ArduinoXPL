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

end)

lmc_on_xpl_var_change('sim/cockpit2/radios/indicators/gps_nav_id', function(value)
  lmc_send_to_com('COM', 'GP1_' .. value .. '|')
end)

lmc_on_xpl_var_change('sim/cockpit2/radios/indicators/gps_dme_distance_nm', function(value, count)
  lmc_send_to_com('COM', 'GPD_' .. string.format("%.2f", value) .. '|')
end, 2000)


os.execute("timeout " .. tonumber(6))   