function bytes = ImageHeaderToBytes(hdr)
% Convert to an ISMRMRD ImageHeader struct to a byte array.

    bytes = zeros(214,1,'int8');
    off = 1;
    bytes(off:off+1)   = typecast(hdr.version               ,'int8'); off=off+2;
    bytes(off:off+7)   = typecast(hdr.flags                 ,'int8'); off=off+8;
    bytes(off:off+3)   = typecast(hdr.measurement_uid       ,'int8'); off=off+4;
    bytes(off:off+5)   = typecast(hdr.matrix_size           ,'int8'); off=off+6;
    bytes(off:off+11)  = typecast(hdr.field_of_view         ,'int8'); off=off+12;
    bytes(off:off+1)   = typecast(hdr.channels              ,'int8'); off=off+2;
    bytes(off:off+11)  = typecast(hdr.position              ,'int8'); off=off+12;
    bytes(off:off+11)  = typecast(hdr.read_dir              ,'int8'); off=off+12;
    bytes(off:off+11)  = typecast(hdr.phase_dir             ,'int8'); off=off+12;
    bytes(off:off+11)  = typecast(hdr.slice_dir             ,'int8'); off=off+12;
    bytes(off:off+11)  = typecast(hdr.patient_table_position,'int8'); off=off+12;
    bytes(off:off+1)   = typecast(hdr.average               ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.slice                 ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.contrast              ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.phase                 ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.repetition            ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.set                   ,'int8'); off=off+2;
    bytes(off:off+3)   = typecast(hdr.acquisition_time_stamp,'int8'); off=off+4;
    bytes(off:off+31)  = typecast(hdr.physiology_time_stamp ,'int8'); off=off+32;
    bytes(off:off+1)   = typecast(hdr.image_data_type       ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.image_type            ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.image_index           ,'int8'); off=off+2;
    bytes(off:off+1)   = typecast(hdr.image_series_index    ,'int8'); off=off+2;
    bytes(off:off+31)  = typecast(hdr.user_int              ,'int8'); off=off+32;
    bytes(off:off+31)  = typecast(hdr.user_float            ,'int8');

end
