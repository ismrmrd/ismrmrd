function bytes = AcquisitionHeaderToBytes(hdr)
% Convert to an ISMRMRD AcquisitionHeader struct to a byte array.

    % TODO: physiology_time_stamp should be 3.
    %bytes = zeros(340,1,'int8');
    bytes = zeros(360,1,'int8');
    off = 1;
    bytes(off:off+1)   = typecast(hdr.version               ,'int8'); off=off+2;
    bytes(off:off+7)   = typecast(hdr.flags                 ,'int8'); off=off+8;
    bytes(off:off+3)   = typecast(hdr.measurement_uid       ,'int8'); off=off+4;
    bytes(off:off+3)   = typecast(hdr.scan_counter          ,'int8'); off=off+4;
    bytes(off:off+3)   = typecast(hdr.acquisition_time_stamp,'int8'); off=off+4;
    
    % TODO: physiology_time_stamp should be 3.
    % but the C struct has a bug, so convert to padding.
    bytes(off:off+11)  = typecast(hdr.physiology_time_stamp ,'int8'); off=off+12;
    off = off+20; % Discard 5*uint32;

    bytes(off:off+1)   = typecast(hdr.number_of_samples     ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.available_channels    ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.active_channels       ,'int8'); off=off+2;
    bytes(off:off+127) = typecast(hdr.channel_mask          ,'int8'); off=off+128;
    bytes(off:off+1)   = typecast(hdr.discard_pre           ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.discard_post          ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.center_sample         ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.encoding_space_ref    ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.trajectory_dimensions ,'int8'); off=off+2;
    bytes(off:off+3)   = typecast(hdr.sample_time_us        ,'int8'); off=off+4;
    bytes(off:off+11)  = typecast(hdr.position              ,'int8'); off=off+12;
    bytes(off:off+11)  = typecast(hdr.read_dir              ,'int8'); off=off+12;
    bytes(off:off+11)  = typecast(hdr.phase_dir             ,'int8'); off=off+12;
    bytes(off:off+11)  = typecast(hdr.slice_dir             ,'int8'); off=off+12;
    bytes(off:off+11)  = typecast(hdr.patient_table_position,'int8'); off=off+12;
    bytes(off:off+1)   = typecast(hdr.idx.kspace_encode_step_1,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.idx.kspace_encode_step_2,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.idx.average           ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.idx.slice             ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.idx.contrast          ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.idx.phase             ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.idx.repetition        ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.idx.set               ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.idx.segment           ,'int8'); off=off+2;
    bytes(off:off+15)  = typecast(hdr.idx.user              ,'int8'); off=off+16;
    bytes(off:off+31)  = typecast(hdr.user_int              ,'int8'); off=off+32;
    bytes(off:off+31)  = typecast(hdr.user_float            ,'int8');

end
