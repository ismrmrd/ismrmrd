function hdr = AcquisitionHeader()
% Return an initialized ISMRMRD AcquisitionHeader

hdr = struct( ...
    'version',                uint16(0),            ... % First unsigned int indicates the version %
    'flags',                  uint64(0),            ... % bit field with flags %
    'measurement_uid',        uint32(0),            ... % Unique ID for the measurement %
    'scan_counter',           uint32(0),            ... % Current acquisition number in the measurement %
    'acquisition_time_stamp', uint32(0),            ... % Acquisition clock %
    'physiology_time_stamp',  zeros(3,1,'uint32'),  ... % Physiology time stamps, e.g. ecg, breating, etc. %
                                                    ... %   TODO: the C header has a bug.  3 is correct
    'number_of_samples',      uint16(0),            ... % Number of samples acquired %
    'available_channels',     uint16(0),            ... % Available coils %
    'active_channels',        uint16(0),            ... % Active coils on current acquisiton %
    'channel_mask',           zeros(16,1,'uint64'), ... % Mask to indicate which channels are active. Support for 1024 channels %
    'discard_pre',            uint16(0),            ... % Samples to be discarded at the beginning of acquisition %
    'discard_post',           uint16(0),            ... % Samples to be discarded at the end of acquisition %
    'center_sample',          uint16(0),            ... % Sample at the center of k-space %
    'encoding_space_ref',     uint16(0),            ... % Reference to an encoding space, typically only one per acquisition %
    'trajectory_dimensions',  uint16(0),            ... % Indicates the dimensionality of the trajectory vector (0 means no trajectory) %
    'sample_time_us',         single(0),            ... % Time between samples in micro seconds, sampling BW %
    'position',               zeros(3,1,'single'),  ... % Three-dimensional spatial offsets from isocenter %
    'read_dir',               zeros(3,1,'single'),  ... % Directional cosines of the readout/frequency encoding %
    'phase_dir',              zeros(3,1,'single'),  ... % Directional cosines of the phase encoding %
    'slice_dir',              zeros(3,1,'single'),  ... % Directional cosines of the slice %
    'patient_table_position', zeros(3,1, 'single'), ... % Patient table off-center %
    'idx',                    struct(),             ... % Encoding counters %
    'user_int',               zeros(8,1,'int32'),   ... % Free user parameters %
    'user_float',             zeros(8,1,'single')   ... % Free user parameters %
    );

    
% Encoding Counters
hdr.idx = struct( ...
    'kspace_encode_step_1',   uint16(0),            ... % phase encoding line number %
    'kspace_encode_step_2',   uint16(0),            ... % partition encodning number %
    'average',                uint16(0),            ... % signal average number %
    'slice',                  uint16(0),            ... % imaging slice number %
    'contrast',               uint16(0),            ... % echo number in multi-echo %
    'phase',                  uint16(0),            ... % cardiac phase number %
    'repetition',             uint16(0),            ... % dynamic number for dynamic scanning %
    'set',                    uint16(0),            ... % flow encoding set %
    'segment',                uint16(0),            ... % segment number for segmented acquisition %
    'user',                   zeros(8,1,'uint16')   ... % Free user parameters %
    );

end
