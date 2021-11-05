<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns="http://www.w3.org/1999/xhtml">

<xsl:variable name="types">
                <xsl:for-each select="/xs:schema/xs:complexType">
                  <xsl:value-of select="@name"/>
                  <xsl:if test="position()!=last()">
                    <xsl:value-of select="'|'"/>
                  </xsl:if>
                </xsl:for-each>
</xsl:variable>

<xsl:variable name="simpletypes">
                <xsl:for-each select="/xs:schema/xs:simpleType">
                  <xsl:value-of select="@name"/>
                  <xsl:if test="position()!=last()">
                    <xsl:value-of select="'|'"/>
                  </xsl:if>
                </xsl:for-each>
</xsl:variable>

<xsl:template match="/xs:schema">
<svg preserveAspectRatio="xMidYMin meet" y="0"  viewBox="0 0 2000 5500" width="100%"  height="600%" xmlns="http://www.w3.org/2000/svg">
  <foreignObject width="100%" height="100%">
    <div xmlns="http://www.w3.org/1999/xhtml">
<head>
    <meta charset="utf-8"></meta>
    <title>MRD header schema documentation</title>
	<style type="text/css">
		html {
				font-family: sans-serif;
			}
				td, th {
				  border: 0.05rem solid rgb(0,0,0);
				  padding: 0.25rem 0.5rem;
				}
				td {
				  text-align: center;
				  word-wrap: break-word;
				  overflow-wrap: break-word;
				  vertical-align: top;
				}
				table {
				  border-collapse: collapse;
				  border: 0.1rem solid rgb(0,0,0);
				  letter-spacing: 0.05rem;
				  font-size: 0.8rem;
				}

				thead th { 
				text-align:left; 
				background:black; 
				color:white
				}

				tbody tr:nth-child(odd) > td,
				tfoot tr:nth-child(odd) > td,
				tr:nth-child(odd) > td
				{ 
				background:rgb(238, 238, 238); 
				}

				tbody tr:nth-child(even) > td,
				tfoot tr:nth-child(even) > td,
				tr:nth-child(even) > td
				{ 
				background:rgb(215, 215, 215);
				}

				tbody table tbody tr:nth-child(odd) > td,
				tbody table tfoot tr:nth-child(odd) > td,
				tbody table tr:nth-child(odd) > td
				{ 
				background:rgb(255, 255, 99) ; 
				}

				tbody table tbody tr:nth-child(even) > td,
				tbody table tfoot tr:nth-child(even) > td,
				tbody table tr:nth-child(even) > td
				{ 
				background:rgb(229, 220, 59);
				}

				tbody table tbody table tbody tr:nth-child(odd) > td,
				tbody table tbody table tbody tfoot tr:nth-child(odd) > td,
				tbody table tbody table tbody tr:nth-child(odd) > td
				{ 
				background:rgb(40, 255, 255) 
				}

				tbody table tbody table tbody tr:nth-child(even) > td,
				tbody table tbody table tbody tfoot tr:nth-child(even) > td,
				tbody table tbody table tbody tr:nth-child(even) > td
				{ 
				background:rgb(20, 207, 235) 
				}

				tbody table tbody table tbody table tbody tr:nth-child(odd) > td,
				tbody table tbody table tbody table tbody tfoot tr:nth-child(odd) > td,
				tbody table tbody table tbody table tbody tr:nth-child(odd) > td
				{
				background:rgb(255, 123, 221) ; 
				}

				tbody table tbody table tbody table tbody tr:nth-child(even) > td,
				tbody table tbody table tbody table tbody tfoot tr:nth-child(even) > td,
				tbody table tbody table tbody table tbody tr:nth-child(even) > td
				{
				background:rgb(255, 83, 191) ; 
				}
	</style>
</head>
<body>
	<xsl:apply-templates select="xs:element"/>
</body>
</div>
  </foreignObject>
</svg>
</xsl:template>

<xsl:template match="xs:complexType">
  <td> 
  <table>
        <thead>
            <tr>
                <th>Name</th>
                <th>Type</th>
                <th>Multiplicity</th>
				<xsl:variable name="annotationNodes"><xsl:value-of select="descendant-or-self::xs:annotation" /></xsl:variable>
				<xsl:if test="not($annotationNodes='')">
					<th>Description</th>
				</xsl:if>
            </tr>
        </thead>
        <tbody>
		<xsl:apply-templates/>
		</tbody>
  </table>
  </td>
</xsl:template>

<xsl:template match="xs:simpleType">
  <td>
  <xsl:value-of select="xs:restriction/@base" />
  </td>
</xsl:template>

<xsl:template match="xs:simpleType/xs:restriction[xs:pattern]">
      Pattern is: <xsl:value-of select="xs:pattern/@value" />
</xsl:template>

<xsl:template match="xs:simpleType/xs:restriction[xs:enumeration]">
      Enumeration is: <xsl:apply-templates select="xs:enumeration"/>
</xsl:template>

<xsl:template match="xs:enumeration">
      <br/> <xsl:value-of select="@value" />
</xsl:template>

<xsl:template match="xs:annotation">
      <xsl:value-of select="xs:documentation" />
</xsl:template>

<xsl:template match="xs:element[@type]">
	<xsl:choose>
	<xsl:when test="@type='ismrmrdHeader'">
		<xsl:for-each select="/xs:schema/xs:complexType">
			<xsl:if test="'ismrmrdHeader'=@name">		
			<table>
				<thead>
					<tr>
						<th>Name</th>
						<th>Type</th>
						<th>Multiplicity</th>
						<th>Description</th>
					</tr>
				</thead>
				<tbody>
				<xsl:apply-templates/>
				</tbody>
			</table>
			</xsl:if>
		</xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
		    <tr>
        <td> <xsl:value-of select="@name" /> </td>
				<xsl:choose>
					<xsl:when test="contains($types, @type)">
					 <xsl:variable name="currentType"><xsl:value-of select="@type" /></xsl:variable>
						<xsl:for-each select="/xs:schema">
								<xsl:apply-templates select="xs:complexType[$currentType=@name]"/>
						</xsl:for-each>
					</xsl:when>
					<xsl:when test="contains($simpletypes, @type)">
					 <xsl:variable name="currentType"><xsl:value-of select="@type" /></xsl:variable>
						<xsl:for-each select="/xs:schema">
								<xsl:apply-templates select="xs:simpleType[$currentType=@name]"/>
						</xsl:for-each>
					</xsl:when>
					<xsl:otherwise>
						<td> <xsl:value-of select="@type" /> </td>
					</xsl:otherwise>
				  </xsl:choose>
				<td>  
			<xsl:choose>
					<xsl:when test="@minOccurs and @maxOccurs and @maxOccurs='unbounded'">
						<xsl:value-of select="@minOccurs" /> - 4096
					</xsl:when>
					<xsl:when test="@minOccurs and @maxOccurs">
						<xsl:choose>
						<xsl:when test="@minOccurs=@maxOccurs">
							<xsl:value-of select="@minOccurs" />
						</xsl:when>
						<xsl:otherwise>
						<xsl:value-of select="@minOccurs" /> - <xsl:value-of select="@maxOccurs" />
					</xsl:otherwise>
				  </xsl:choose>
					</xsl:when>
					<xsl:when test="@minOccurs and @minOccurs='0'">
						<xsl:value-of select="@minOccurs" /> - 1
					</xsl:when>
					<xsl:when test="@minOccurs">
						<xsl:value-of select="@minOccurs" />
					</xsl:when>
					<xsl:otherwise>
						1
					</xsl:otherwise>
				  </xsl:choose>
		</td>
		<xsl:variable name="annotationNodes"><xsl:value-of select="descendant-or-self::xs:annotation" /></xsl:variable>
		<xsl:if test="not($annotationNodes='')">
		<td> 
			<xsl:apply-templates select="xs:annotation"/> 
			<xsl:if test="contains($simpletypes, @type)">
					 <xsl:variable name="currentType"><xsl:value-of select="@type" /></xsl:variable>
						<xsl:for-each select="/xs:schema">
								<xsl:apply-templates select="xs:simpleType[$currentType=@name]/xs:restriction[xs:enumeration]"/>
						</xsl:for-each>
					</xsl:if>
		</td>
		</xsl:if>
	</tr>
	</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="xs:element[not(@type)]">
	<tr>
        <td> <xsl:value-of select="@name" /> </td>			
		<td> <xsl:value-of select="xs:simpleType/xs:restriction/@base" /> </td>
		<td>  
						<xsl:choose>
					<xsl:when test="@minOccurs and @maxOccurs and @maxOccurs='unbounded'">
						<xsl:value-of select="@minOccurs" /> - 4096
					</xsl:when>
					<xsl:when test="@minOccurs and @maxOccurs">
						<xsl:value-of select="@minOccurs" /> - <xsl:value-of select="@maxOccurs" />
					</xsl:when>
					<xsl:when test="@minOccurs and @minOccurs='0'">
						<xsl:value-of select="@minOccurs" /> - 1
					</xsl:when>
					<xsl:when test="@minOccurs">
						<xsl:value-of select="@minOccurs" />
					</xsl:when>
					<xsl:otherwise>
						1
					</xsl:otherwise>
				  </xsl:choose>
		</td>
		<xsl:variable name="annotationNodes"><xsl:value-of select="descendant-or-self::xs:annotation" /></xsl:variable>
		<xsl:if test="not($annotationNodes='')">
			<td> 
				<xsl:apply-templates select="xs:annotation"/>
				<xsl:apply-templates select="xs:simpleType/xs:restriction[xs:pattern]"/>
				<xsl:apply-templates select="xs:simpleType/xs:restriction[xs:enumeration]"/>
			</td>
		</xsl:if>
	</tr>
</xsl:template>

</xsl:stylesheet>