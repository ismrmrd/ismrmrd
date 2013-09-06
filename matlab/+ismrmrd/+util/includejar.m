function includejar()
% Set the javaclasspath to be able to load the xml header bits

    % Path to the jar file
    fpath = mfilename('fullpath');
    [pathstr, name, ext] = fileparts(fpath);
    jar = fullfile(pathstr, 'ismrmrd-matlab.jar');
    
    % Only add if it's not already on our path
    jpath = javaclasspath('-all');
    if ~any(strcmp(jar,jpath))
        javaaddpath(jar);
    end

end
