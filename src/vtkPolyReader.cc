/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPolyReader.cc
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


Copyright (c) 1993-1995 Ken Martin, Will Schroeder, Bill Lorensen.

This software is copyrighted by Ken Martin, Will Schroeder and Bill Lorensen.
The following terms apply to all files associated with the software unless
explicitly disclaimed in individual files. This copyright specifically does
not apply to the related textbook "The Visualization Toolkit" ISBN
013199837-4 published by Prentice Hall which is covered by its own copyright.

The authors hereby grant permission to use, copy, and distribute this
software and its documentation for any purpose, provided that existing
copyright notices are retained in all copies and that this notice is included
verbatim in any distributions. Additionally, the authors grant permission to
modify this software and its documentation for any purpose, provided that
such modifications are not distributed without the explicit consent of the
authors and that existing copyright notices are retained in all copies. Some
of the algorithms implemented by this software are patented, observe all
applicable patent law.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================*/
#include "vtkPolyReader.hh"

vtkPolyReader::vtkPolyReader()
{
}

unsigned long int vtkPolyReader::GetMTime()
{
  unsigned long dtime = this->vtkSource::GetMTime();
  unsigned long rtime = this->Reader.GetMTime();
  return (dtime > rtime ? dtime : rtime);
}

// Description:
// Specify file name of vtk polygonal data file to read.
void vtkPolyReader::SetFilename(char *name) 
{
  this->Reader.SetFilename(name);
}
char *vtkPolyReader::GetFilename() 
{
  return this->Reader.GetFilename();
}

// Description:
// Get the type of file (ASCII or BINARY)
int vtkPolyReader::GetFileType() 
{
  return this->Reader.GetFileType();
}

// Description:
// Set the name of the scalar data to extract. If not specified, first 
// scalar data encountered is extracted.
void vtkPolyReader::SetScalarsName(char *name) 
{
  this->Reader.SetScalarsName(name);
}
char *vtkPolyReader::GetScalarsName() 
{
  return this->Reader.GetScalarsName();
}

// Description:
// Set the name of the vector data to extract. If not specified, first 
// vector data encountered is extracted.
void vtkPolyReader::SetVectorsName(char *name) 
{
  this->Reader.SetVectorsName(name);
}
char *vtkPolyReader::GetVectorsName() 
{
  return this->Reader.GetVectorsName();
}

// Description:
// Set the name of the tensor data to extract. If not specified, first 
// tensor data encountered is extracted.
void vtkPolyReader::SetTensorsName(char *name) 
{
  this->Reader.SetTensorsName(name);
}
char *vtkPolyReader::GetTensorsName() 
{
  return this->Reader.GetTensorsName();
}

// Description:
// Set the name of the normal data to extract. If not specified, first 
// normal data encountered is extracted.
void vtkPolyReader::SetNormalsName(char *name) 
{
  this->Reader.SetNormalsName(name);
}
char *vtkPolyReader::GetNormalsName() 
{
  return this->Reader.GetNormalsName();
}

// Description:
// Set the name of the texture coordinate data to extract. If not specified,
// first texture coordinate data encountered is extracted.
void vtkPolyReader::SetTCoordsName(char *name) 
{
  this->Reader.SetTCoordsName(name);
}
char *vtkPolyReader::GetTCoordsName() 
{
  return this->Reader.GetTCoordsName();
}

// Description:
// Set the name of the lookup table data to extract. If not specified, uses 
// lookup table named by scalar. Otherwise, this specification supersedes.
void vtkPolyReader::SetLookupTableName(char *name) 
{
  this->Reader.SetLookupTableName(name);
}
char *vtkPolyReader::GetLookupTableName() 
{
  return this->Reader.GetLookupTableName();
}

void vtkPolyReader::Execute()
{
  FILE *fp;
  int numPts=0;
  int retStat;
  char line[257];
  int npts, size, ncells;
  vtkPolyData *output=(vtkPolyData *)this->Output;

  vtkDebugMacro(<<"Reading vtk polygonal data...");

  if ( this->Debug ) this->Reader.DebugOn();
  else this->Reader.DebugOff();

  if ( !(fp=this->Reader.OpenVTKFile()) ||
  ! this->Reader.ReadHeader(fp) )
      return;
//
// Read polygonal data specific stuff
//
  if ( (retStat=fscanf(fp,"%256s",line)) == EOF || retStat < 1 ) 
    {
    vtkErrorMacro(<<"Data file ends prematurely!");
    return;
    }

  if ( !strncmp(this->Reader.LowerCase(line),"dataset",(unsigned long)7) )
    {
//
// Make sure we're reading right type of geometry
//
    if ( (retStat=fscanf(fp,"%256s",line)) == EOF || retStat < 1 ) 
      {
      vtkErrorMacro(<<"Data file ends prematurely!");
      return;
      } 

    if ( strncmp(this->Reader.LowerCase(line),"polydata",8) )
      {
      vtkErrorMacro(<< "Cannot read dataset type: " << line);
      return;
      }
//
// Might find points, vertices, lines, polygons, or triangle strips
//
    while (1)
      {
      if ( (retStat=fscanf(fp,"%256s",line)) == EOF || retStat < 1 ) break;

      if ( ! strncmp(this->Reader.LowerCase(line),"points",6) )
        {
        if ( (retStat=fscanf(fp,"%d", &numPts)) == EOF || retStat < 1 ) 
          {
          vtkErrorMacro(<<"Cannot read number of points!");
          return;
          }

        this->Reader.ReadPoints(fp, (vtkPointSet *)this, numPts);
        }

      else if ( ! strncmp(line,"vertices",8) )
        {
        vtkCellArray *verts = new vtkCellArray;
        if ( (retStat=fscanf(fp,"%d %d", &ncells, &size)) == EOF || retStat < 2 ) 
          {
          vtkErrorMacro(<<"Cannot read vertices!");
          return;
          }

        this->Reader.ReadCells(fp, size, verts->WritePtr(ncells,size));
        verts->WrotePtr();
        output->SetVerts(verts);
        verts->Delete();
        vtkDebugMacro(<<"Read " << ncells << " vertices");
        }

      else if ( ! strncmp(line,"lines",5) )
        {
        vtkCellArray *lines = new vtkCellArray;
        if ( (retStat=fscanf(fp,"%d %d", &ncells, &size)) == EOF || retStat < 2 ) 
          {
          vtkErrorMacro(<<"Cannot read lines!");
          return;
          }

        this->Reader.ReadCells(fp, size, lines->WritePtr(ncells,size));
        lines->WrotePtr();
        output->SetLines(lines);
        lines->Delete();
        vtkDebugMacro(<<"Read " << ncells << " lines");
        }

      else if ( ! strncmp(line,"polygons",8) )
        {
        vtkCellArray *polys = new vtkCellArray;
        if ( (retStat=fscanf(fp,"%d %d", &ncells, &size)) == EOF || retStat < 2 ) 
          {
          vtkErrorMacro(<<"Cannot read polygons!");
          return;
          }

        this->Reader.ReadCells(fp, size, polys->WritePtr(ncells,size));
        polys->WrotePtr();
        output->SetPolys(polys);
        polys->Delete();
        vtkDebugMacro(<<"Read " << ncells << " polygons");
        }

      else if ( ! strncmp(line,"triangle_strips",15) )
        {
        vtkCellArray *tris = new vtkCellArray;
        if ( (retStat=fscanf(fp,"%d %d", &ncells, &size)) == EOF || retStat < 2 ) 
          {
          vtkErrorMacro(<<"Cannot read triangle strips!");
          return;
          }

        this->Reader.ReadCells(fp, size, tris->WritePtr(ncells,size));
        tris->WrotePtr();
        output->SetStrips(tris);
        tris->Delete();
        vtkDebugMacro(<<"Read " << ncells << " triangle strips");
        }

      else if ( ! strncmp(line, "point_data", 10) )
        {
        if ( (retStat=fscanf(fp,"%d", &npts)) == EOF || retStat < 1 ) 
          {
          vtkErrorMacro(<<"Cannot read point data!");
          return;
          }
        
        if ( npts != numPts )
          {
          vtkErrorMacro(<<"Number of points don't match number data values!");
          return;
          }

        this->Reader.ReadPointData(fp, output, npts);
        break; //out of this loop
        }

      else
        {
        vtkErrorMacro(<< "Unrecognized keyord: " << line);
        return;
        }
      }

      if ( ! output->GetPoints() ) vtkWarningMacro(<<"No points read!");
      if ( !(output->GetVerts() || output->GetLines() || 
      output->GetPolys() || output->GetStrips()) ) 
        vtkWarningMacro(<<"No topology read!");
    }

  else if ( !strncmp(line, "point_data", 10) )
    {
    vtkWarningMacro(<<"No geometry defined in data file!");
    if ( (retStat=fscanf(fp,"%d", &numPts)) == EOF || retStat < 1 ) 
      {
      vtkErrorMacro(<<"Cannot read point data!");
      return;
      }

    this->Reader.ReadPointData(fp, output, numPts);
    }

  else 
    {
    vtkErrorMacro(<< "Unrecognized keyord: " << line);
    }
}

void vtkPolyReader::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkPolySource::PrintSelf(os,indent);
  this->Reader.PrintSelf(os,indent);
}
