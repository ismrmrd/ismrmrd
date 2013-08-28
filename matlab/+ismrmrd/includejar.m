function includejar()
    fpath = mfilename('fullpath');
    [pathstr, name, ext] = fileparts(fpath);
    jar = fullfile(pathstr, 'ismrmrd-matlab.jar');
    javaaddpath(jar);
end
