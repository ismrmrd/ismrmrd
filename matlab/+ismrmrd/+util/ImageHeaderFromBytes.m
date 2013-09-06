function hdr = ImageHeaderFromBytes(bytes)
% Construct an ISMRMRD Image Header structure from a byte array.

    off = 1;
    hdr = struct();
    hdr.version                = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.flags                  = typecast(bytes(off:off+7),  'uint64'); off=off+8;
    hdr.measurement_uid        = typecast(bytes(off:off+3),  'uint32'); off=off+4;
    hdr.matrix_size            = typecast(bytes(off:off+5),  'uint16'); off=off+6;
    hdr.field_of_view          = typecast(bytes(off:off+11), 'single'); off=off+12;
    hdr.channels               = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.position               = typecast(bytes(off:off+11), 'single'); off=off+12;
    hdr.read_dir               = typecast(bytes(off:off+11), 'single'); off=off+12;
    hdr.phase_dir              = typecast(bytes(off:off+11), 'single'); off=off+12;
    hdr.slice_dir              = typecast(bytes(off:off+11), 'single'); off=off+12;
    hdr.patient_table_position = typecast(bytes(off:off+11), 'single'); off=off+12;
    hdr.average                = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.slice                  = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.contrast               = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.phase                  = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.repetition             = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.set                    = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.acquisition_time_stamp = typecast(bytes(off:off+3),  'uint32'); off=off+4;
    hdr.physiology_time_stamp  = typecast(bytes(off:off+31), 'uint32'); off=off+32;
    hdr.image_data_type        = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.image_type             = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.image_index            = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.image_series_index     = typecast(bytes(off:off+1),  'uint16'); off=off+2;
    hdr.user_int               = typecast(bytes(off:off+31), 'uint32'); off=off+32;
    hdr.user_float             = typecast(bytes(off:off+31), 'single');

end