print('ArduinoXPL')

-- ArduinoXPL - COM
lmc_add_com('COM', 'COM4', 9600, 8, 'N', 1)
lmc_set_com_splitter('COM', '\n')

-- zpracovani prijatych dat z COM
lmc_set_handler('COM', function(comVal)
  -- print(">>" .. comVal .. "<<")

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

end)