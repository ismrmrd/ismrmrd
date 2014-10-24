function [header] = deserialize(xmlstring)
%DESERIALIZE Summary of this function goes here
%   Detailed explanation goes here

    % Get a parser
    db = javax.xml.parsers.DocumentBuilderFactory.newInstance().newDocumentBuilder();

    % Turn the string into a stream
    isrc = org.xml.sax.InputSource();
    isrc.setCharacterStream(java.io.StringReader(xmlstring))

    % Parse it
    dom = db.parse(isrc);

    % Get the root element
    rootNode = dom.getDocumentElement();

    % Fill it
    header = parseNode(rootNode);

end

% ----- Subfunction parseChild -----
function info = parseNode(theNode)

% Walk down the tree
childNodes = getChildNodes(theNode);
numChildNodes = getLength(childNodes);

info = struct;

for n = 1:numChildNodes
    theChild = item(childNodes,n-1);
    name = char(getNodeName(theChild));

    %Some elements occure more than once
    if isfield(info,name)
        num = length(info.(name))+1;
    else
        num = 1;
    end

    if strcmp(name, 'encoding')
        if num == 1
            info.encoding = struct('encodedSpace',struct,'reconSpace',struct, ...
                'encodingLimits', struct, 'trajectory', '', ...
                'trajectoryDescription', struct,  ...
                'parallelImaging', struct, 'echoTrainLength', 0);
        end
        temp = parseNode(theChild);
        fnames = fieldnames(temp);
        for f = 1:length(fnames)
            info.encoding(num).(fnames{f}) = temp.(fnames{f});
        end
        continue;
    end
    
    if isCompoundType(name)        
        if num == 1
            info.(name) = parseNode(theChild);
        else
            info.(name)(num) = parseNode(theChild);
        end
        continue;
    end

    if isUserParameterType(name)
        if num == 1
            info.(name) = parseUserParameter(theChild);
        else
            info.(name)(num) = parseUserParameter(theChild);
        end
        continue;
    end

    if isStringType(name)
        if num == 1
            info.(name) = char(getTextContent(theChild));
        else
            info.(name)(num) = char(getTextContent(theChild));
        end
        continue;
    end

    if isNumericalType(name)
        if num == 1
            info.(name) = str2num(getTextContent(theChild));
        else
            info.(name)(num) = str2num(getTextContent(theChild));
        end
        continue;
    end

    if isDateType(name)
        if num == 1
            info.(name) = char(getTextContent(theChild));
        else
            info.(name)(num) = char(getTextContent(theChild));
        end
        continue;
    end

end

end

%%%%%%%%%%%%%%%%%%%
function info = parseUserParameter(theNode)

    paramType = char(getNodeName(theNode));
    childNodes = getChildNodes(theNode);
    numChildNodes = getLength(childNodes);
    
    info = struct;
    
    for n = 1:numChildNodes
        theChild = item(childNodes,n-1);
        if strcmp(getNodeName(theChild),'name')
            info.name = char(getTextContent(theChild));
        end
        if strcmp(getNodeName(theChild),'value')
            if strcmp(paramType, 'userParameterLong')
                info.value = str2num(getTextContent(theChild));
            end

            if strcmp(paramType, 'userParameterDouble')
                info.value = str2num(getTextContent(theChild));
            end

            if strcmp(paramType, 'userParameterString')
                info.value = char(getTextContent(theChild));
            end

            if strcmp(paramType, 'userParameterBase64')
                info.value = char(getTextContent(theChild));
            end
        end
    end
end

% ----- Type specific functions ----
function status = isCompoundType(name)

    % treat encoding separately
    headerNodeNames = { ...
        'version', ...
        'subjectInformation', ...
        'studyInformation', ...
        'measurementInformation', ...
        'acquisitionSystemInformation', ...
        'experimentalConditions', ...
        'encoding', ...
        'sequenceParameters', ...
        'userParameters', ...
        'measurementDependency', ...
        'referencedImageSequence', ...
        'encodedSpace', ...
        'reconSpace', ...
        'encodingLimits', ...
        'trajectoryDescription', ...
        'echoTrainLength', ...
        'matrixSize', ...
        'fieldOfView_mm', ...
        'kspace_encoding_step_0', ...
        'kspace_encoding_step_1', ...
        'kspace_encoding_step_2', ...
        'average', ...
        'slice', ...
        'contrast', ...
        'phase', ...
        'repetition', ...
        'set', ...
        'segment'};
    
    status = ismember(name, headerNodeNames);
end

function status = isNumericalType(name)
    headerNumericalTypes = { ...
      'version', ...
      'patientWeight', ...
      'accessionNumber', ...
      'initialSeriesNumber', ...
      'systemFieldStrength_T', ...
      'relativeReceiverNoiseBandwidth', ...
      'receiverChannels', ...
      'H1resonanceFrequency_Hz', ...
      'TR', ...
      'TE', ...
      'TI', ...
      'flipAngle_deg', ...
      'x', 'y', 'z', ...
      'minimum', 'maximum', 'center'};
  
    status = ismember(name, headerNumericalTypes);
end

function status = isStringType(name)
    headerStringTypes = {...
      'patientName', ...
      'patientID', ...
      'patientGender', ...
      'studyID', ...
      'referringPhysicianName', ...
      'studyDescription', ...
      'studyInstanceUID', ...
      'measurementID', ...
      'patientPosition', ...
      'protocolName', ...
      'seriesDescription', ...
      'seriesInstanceUIDRoot', ...
      'frameOfReferenceUID', ...
      'referencedSOPInstanceUID', ...
      'dependencyType', ...
      'measurementID', ...
      'systemVendor', ...
      'systemModel', ...
      'institutionName', ...
      'stationName', ...
      'trajectory'};
      %'calibrationMode',...
      %'interleavingDimension'};

      status = ismember(name, headerStringTypes);
end

function status = isDateType(name)
    headerDateTypes = {...
      'patientBirthdate', ...
      'studyDate', ...
      'studyTime', ...
      'seriesDate', ...
      'seriesTime'};
    status = ismember(name, headerDateTypes);
end

function status = isUserParameterType(name)
    typeNames =  { ...
        'userParameterLong', ...
        'userParameterDouble', ...
        'userParameterString', ....
        'userParameterBase64'};

    status = ismember(name, typeNames);
end
% 
%   <xs:complexType name="encoding">
%     <xs:all>
%       <xs:element maxOccurs="1" minOccurs="1" name="encodedSpace" type="encodingSpaceType"/>
%       <xs:element maxOccurs="1" minOccurs="1" name="reconSpace" type="encodingSpaceType"/>
%       <xs:element maxOccurs="1" minOccurs="1" name="encodingLimits" type="encodingLimitsType"/>
%       <xs:element maxOccurs="1" minOccurs="1" name="trajectory" type="trajectoryType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="trajectoryDescription" type="trajectoryDescriptionType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="parallelImaging" type="parallelImagingType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="echoTrainLength" type="xs:long"/>
%     </xs:all>
%   </xs:complexType>
% 
%   <xs:complexType name="encodingSpaceType">
%     <xs:all>
%       <xs:element maxOccurs="1" minOccurs="1" name="matrixSize" type="matrixSize"/>
%       <xs:element maxOccurs="1" minOccurs="1" name="fieldOfView_mm" type="fieldOfView_mm"/>
%     </xs:all>
%   </xs:complexType>
% 
%   <xs:complexType name="matrixSize">
%     <xs:sequence>
%       <xs:element default="1" maxOccurs="1" minOccurs="1" name="x" type="xs:unsignedShort"/>
%       <xs:element default="1" maxOccurs="1" minOccurs="1" name="y" type="xs:unsignedShort"/>
%       <xs:element default="1" maxOccurs="1" minOccurs="1" name="z" type="xs:unsignedShort"/>
%     </xs:sequence>
%   </xs:complexType>
% 
%   <xs:complexType name="fieldOfView_mm">
%     <xs:sequence>
%       <xs:element maxOccurs="1" minOccurs="1" name="x" type="xs:float"/>
%       <xs:element maxOccurs="1" minOccurs="1" name="y" type="xs:float"/>
%       <xs:element maxOccurs="1" minOccurs="1" name="z" type="xs:float"/>
%     </xs:sequence>
%   </xs:complexType>
% 
%   <xs:complexType name="limitType">
%     <xs:all>
%       <xs:element default="0" name="minimum" type="xs:unsignedShort"/>
%       <xs:element default="0" name="maximum" type="xs:unsignedShort"/>
%       <xs:element default="0" name="center" type="xs:unsignedShort"/>
%     </xs:all>
%   </xs:complexType>
% 
%   <xs:complexType name="encodingLimitsType">
%     <xs:all>
%       <xs:element maxOccurs="1" minOccurs="0" name="kspace_encoding_step_0" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="kspace_encoding_step_1" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="kspace_encoding_step_2" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="average" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="slice" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="contrast" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="phase" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="repetition" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="set" type="limitType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="segment" type="limitType"/>
%     </xs:all>
%   </xs:complexType>
% 
%   <xs:simpleType name="trajectoryType">
%     <xs:restriction base="xs:string">
%       <xs:enumeration value="cartesian"/>
%       <xs:enumeration value="epi"/>
%       <xs:enumeration value="radial"/>
%       <xs:enumeration value="goldenangle"/>
%       <xs:enumeration value="spiral"/>
%       <xs:enumeration value="other"/>
%     </xs:restriction>
%   </xs:simpleType>
% 
%   <xs:complexType name="trajectoryDescriptionType">
%     <xs:sequence>
%       <xs:element maxOccurs="1" minOccurs="1" name="identifier" type="xs:string"/>
%       <xs:element maxOccurs="unbounded" minOccurs="0" name="userParameterLong" type="userParameterLongType"/>
%       <xs:element maxOccurs="unbounded" minOccurs="0" name="userParameterDouble" type="userParameterDoubleType"/>
%       <xs:element maxOccurs="1" minOccurs="0" name="comment" type="xs:string"/>
%     </xs:sequence>
%   </xs:complexType>

% 
%   <xs:complexType name="userParameterLongType">
%     <xs:all>
%       <xs:element name="name" type="xs:string"/>
%       <xs:element name="value" type="xs:long"/>
%     </xs:all>
%   </xs:complexType>
% 
%   <xs:complexType name="userParameterDoubleType">
%     <xs:all>
%       <xs:element name="name" type="xs:string"/>
%       <xs:element name="value" type="xs:double"/>
%     </xs:all>
%   </xs:complexType>
% 
%   <xs:complexType name="userParameterStringType">
%     <xs:all>
%       <xs:element name="name" type="xs:string"/>
%       <xs:element name="value" type="xs:string"/>
%     </xs:all>
%   </xs:complexType>
% 
%   <xs:complexType name="userParameterBase64Type">
%     <xs:all>
%       <xs:element name="name" type="xs:string"/>
%       <xs:element name="value" type="xs:base64Binary"/>
%     </xs:all>
%   </xs:complexType>
