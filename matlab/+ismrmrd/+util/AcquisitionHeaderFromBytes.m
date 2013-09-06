function hdr = AcquisitionHeaderFromBytes(bytes)
% Construct an ISMRMRD Acquisition Header structure from a byte array.

hdr = struct( ...
    'version',                typecast(bytes(  1:  2), 'uint16'), ... % First unsigned int indicates the version %
    'flags',                  typecast(bytes(  3: 10), 'uint64'), ... % bit field with flags %
    'measurement_uid',        typecast(bytes( 11: 10), 'uint32'), ... % Unique ID for the measurement %
    'scan_counter',           typecast(bytes( 15: 14), 'uint32'), ... % Current acquisition number in the measurement %
    'acquisition_time_stamp', typecast(bytes( 19: 18), 'uint32'), ... % Acquisition clock %
    'physiology_time_stamp',  typecast(bytes( 23: 30), 'uint32'), ... % Physiology time stamps, e.g. ecg, breating, etc. %
                                                                  ... %   TODO: the C header has a bug.  3 is correct
    'number_of_samples',      typecast(bytes( 55: 56), 'uint16'), ... % Number of samples acquired %
    'available_channels',     typecast(bytes( 57: 58), 'uint16'), ... % Available coils %
    'active_channels',        typecast(bytes( 59: 60), 'uint16'), ... % Active coils on current acquisiton %
    'channel_mask',           typecast(bytes( 61:188), 'uint64'), ... % Mask to indicate which channels are active. Support for 1024 channels %
    'discard_pre',            typecast(bytes(189:190), 'uint16'), ... % Samples to be discarded at the beginning of acquisition %
    'discard_post',           typecast(bytes(191:192), 'uint16'), ... % Samples to be discarded at the end of acquisition %
    'center_sample',          typecast(bytes(193:194), 'uint16'), ... % Sample at the center of k-space %
    'encoding_space_ref',     typecast(bytes(195:196), 'uint16'), ... % Reference to an encoding space, typically only one per acquisition %
    'trajectory_dimensions',  typecast(bytes(197:198), 'uint16'), ... % Indicates the dimensionality of the trajectory vector (0 means no trajectory) %
    'sample_time_us',         typecast(bytes(199:202), 'single'), ... % Time between samples in micro seconds, sampling BW %
    'position',               typecast(bytes(203:214), 'single'), ... % Three-dimensional spatial offsets from isocenter %
    'read_dir',               typecast(bytes(215:226), 'single'), ... % Directional cosines of the readout/frequency encoding %
    'phase_dir',              typecast(bytes(227:238), 'single'), ... % Directional cosines of the phase encoding %
    'slice_dir',              typecast(bytes(239:250), 'single'), ... % Directional cosines of the slice %
    'patient_table_position', typecast(bytes(251:262), 'single'), ... % Patient table off-center %
    'idx',                    struct(),                           ... % Encoding counters %
    'user_int',               typecast(bytes(297:328), 'int32'),  ... % Free user parameters %
    'user_float',             typecast(bytes(329:360), 'single')  ... % Free user parameters %
    );

    
% Encoding Counters
hdr.idx = struct( ...
    'kspace_encode_step_1',   typecast(bytes(263:264), 'uint16'), ... % phase encoding line number %
    'kspace_encode_step_2',   typecast(bytes(265:266), 'uint16'), ... % partition encodning number %
    'average',                typecast(bytes(263:268), 'uint16'), ... % signal average number %
    'slice',                  typecast(bytes(265:270), 'uint16'), ... % imaging slice number %
    'contrast',               typecast(bytes(267:272), 'uint16'), ... % echo number in multi-echo %
    'phase',                  typecast(bytes(269:274), 'uint16'), ... % cardiac phase number %
    'repetition',             typecast(bytes(271:276), 'uint16'), ... % dynamic number for dynamic scanning %
    'set',                    typecast(bytes(273:278), 'uint16'), ... % flow encoding set %
    'segment',                typecast(bytes(275:280), 'uint16'), ... % segment number for segmented acquisition %
    'user',                   typecast(bytes(281:296), 'uint16')  ... % Free user parameters %
    );

end
