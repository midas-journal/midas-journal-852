#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkShapeLabelObject.h"
#include "itkShapeLabelMapFilter.h"
#include "itkTimeProbe.h"

#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
 
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkMassProperties.h>
#include <vtkTriangleFilter.h>
#include <vtkImageGaussianSmooth.h>
#include <iomanip>

#include "itkBinaryMask3DMeshSource.h"
#include "itkTriangleHelper.h"

int main(int argc, char *argv[])
{
    std::cout 
//               << "bi2lm" << "\t" 
//               << "shape" << "\t" 
//               << "smooth" << "\t" 
//               << "march" << "\t" 
//               << "trian" << "\t" 
//               << "mass" << "\t" 
//               << "mesh" << "\t" 
//               << "isurf" << "\t" 
              << "Tcroft" << "\t"
              << "Tmarch" << "\t"
              << "Tmarch2" << "\t"
              << "Timesh" << "\t"
              << "speedupMarch" << "\t"
              << "speedupMarch2" << "\t"
              << "speedupIMesh" << "\t"
              << "Scroft" << "\t"
              << "Smarch" << "\t"
              << "Smarch2" << "\t"
              << "Simesh" << "\t"
              << "%march" << "\t"
              << "%march2" << "\t"
              << "%imesh" << "\t"
              << "fname" << "\t"
              << std::endl;
  for( int fi=1; fi<argc; fi++ )
    {
    const int dim = 3;
    typedef unsigned char PType;
    typedef itk::Image< PType, dim > IType;

    typedef itk::ImageFileReader< IType > ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[fi] );
    reader->Update();    

    typedef itk::ShapeLabelObject< unsigned long, 3 > LOType;
    typedef itk::LabelMap< LOType > LabelMapType;
    typedef itk::LabelImageToLabelMapFilter< IType, LabelMapType > BI2LMType;
    BI2LMType::Pointer bi2lm = BI2LMType::New();
    bi2lm->SetInput( reader->GetOutput() );

    typedef itk::ShapeLabelMapFilter< LabelMapType > ShapeType;
    ShapeType::Pointer shape = ShapeType::New();
    shape->SetInput( bi2lm->GetOutput() );
    
    typedef itk::ImageToVTKImageFilter< IType > ConverterType;
    ConverterType::Pointer converter = ConverterType::New();
    converter->SetInput( reader->GetOutput() );
    converter->Update();

    vtkSmartPointer<vtkImageGaussianSmooth> smoothing = 
      vtkSmartPointer<vtkImageGaussianSmooth>::New();
    smoothing->SetInput(converter->GetOutput());
    smoothing->SetStandardDeviation( 1.0 );

    vtkSmartPointer<vtkMarchingCubes> surface = 
        vtkSmartPointer<vtkMarchingCubes>::New();
    surface->SetInput(smoothing->GetOutput());
    surface->ComputeNormalsOff();
    surface->SetValue(0, 128);

    vtkSmartPointer<vtkTriangleFilter> triangle =
        vtkSmartPointer<vtkTriangleFilter>::New();
    triangle->SetInput( surface->GetOutput() );

    vtkSmartPointer<vtkMassProperties> mass =
        vtkSmartPointer<vtkMassProperties>::New();
    mass->SetInput( triangle->GetOutput() );

    vtkSmartPointer<vtkMarchingCubes> surface2 = 
        vtkSmartPointer<vtkMarchingCubes>::New();
    surface2->SetInput(converter->GetOutput());
    surface2->ComputeNormalsOff();
    surface2->SetValue(0, 128);

    vtkSmartPointer<vtkTriangleFilter> triangle2 =
        vtkSmartPointer<vtkTriangleFilter>::New();
    triangle2->SetInput( surface2->GetOutput() );

    vtkSmartPointer<vtkMassProperties> mass2 =
        vtkSmartPointer<vtkMassProperties>::New();
    mass2->SetInput( triangle2->GetOutput() );

    // Declare the type of the Mesh
    typedef itk::Mesh<double>                         MeshType;
    typedef MeshType::PointType                       PointType;
    typedef itk::BinaryMask3DMeshSource< IType, MeshType >   MeshSourceType;
    MeshSourceType::Pointer meshSource = MeshSourceType::New();
    meshSource->SetInput( reader->GetOutput() );
    meshSource->SetObjectValue( 255 );
    double isurf = 0;

    itk::TimeProbe bi2lmTime;
    itk::TimeProbe shapeTime;
    itk::TimeProbe smoothingTime;
    itk::TimeProbe marchingTime;
    itk::TimeProbe triangleTime;
    itk::TimeProbe massTime;
    itk::TimeProbe marching2Time;
    itk::TimeProbe triangle2Time;
    itk::TimeProbe mass2Time;
    itk::TimeProbe imeshTime;
    itk::TimeProbe isurfTime;
    for(int i=0; i<=10; i++ )
      {
      smoothing->Modified();
      smoothingTime.Start();
      smoothing->Update();
      smoothingTime.Stop();
    
      surface->Modified();
      marchingTime.Start();
      surface->Update();
      marchingTime.Stop();
    
      triangle->Modified();
      triangleTime.Start();
      triangle->Update();
      triangleTime.Stop();
    
      mass->Modified();
      massTime.Start();
      mass->Update();
      massTime.Stop();
    
      surface2->Modified();
      marching2Time.Start();
      surface2->Update();
      marching2Time.Stop();
    
      triangle2->Modified();
      triangle2Time.Start();
      triangle2->Update();
      triangle2Time.Stop();
    
      mass2->Modified();
      mass2Time.Start();
      mass2->Update();
      mass2Time.Stop();
    
      bi2lm->Modified();
      bi2lmTime.Start();
      bi2lm->Update();
      bi2lmTime.Stop();
    
      shape->Modified();
      shapeTime.Start();
      shape->Update();
      shapeTime.Stop();
    
      meshSource->Modified();
      imeshTime.Start();
      meshSource->Update();
      imeshTime.Stop();
    
      isurf = 0;
      isurfTime.Start();
      MeshType * mesh = meshSource->GetOutput();
      for( MeshSourceType::OutputMeshType::CellsContainerConstIterator it = mesh->GetCells()->Begin(); it != mesh->GetCells()->End(); ++it )
        {
        const MeshType::CellType::PointIdentifierContainerType & pids = it.Value()->GetPointIdsContainer();
        PointType p0 = mesh->GetPoint(pids[0]);
        PointType p1 = mesh->GetPoint(pids[1]);
        PointType p2 = mesh->GetPoint(pids[2]);
        isurf += itk::TriangleHelper<PointType>::ComputeArea(p0, p1, p2);
        }
      isurfTime.Stop();
      }

      double Tcroft = bi2lmTime.GetMeanTime() + shapeTime.GetMeanTime();
      double Tmarch = smoothingTime.GetMeanTime() + marchingTime.GetMeanTime() + triangleTime.GetMeanTime() + massTime.GetMeanTime();
      double Tmarch2 = marching2Time.GetMeanTime() + triangle2Time.GetMeanTime() + mass2Time.GetMeanTime();
      double Timesh = imeshTime.GetMeanTime() + isurfTime.GetMeanTime();

      std::cout // << std::setprecision(3)
//                 << bi2lmTime.GetMeanTime() << "\t" 
//                 << shapeTime.GetMeanTime() << "\t" 
//                 << smoothingTime.GetMeanTime() << "\t" 
//                 << marchingTime.GetMeanTime() << "\t" 
//                 << triangleTime.GetMeanTime() << "\t" 
//                 << massTime.GetMeanTime() << "\t" 
//                 << imeshTime.GetMeanTime() << "\t" 
//                 << isurfTime.GetMeanTime() << "\t" 
                << Tcroft << "\t" 
                << Tmarch << "\t" 
                << Tmarch2 << "\t" 
                << Timesh << "\t" 
                << Tmarch / Tcroft << "\t" 
                << Tmarch2 / Tcroft << "\t" 
                << Timesh / Tcroft << "\t" 
                << bi2lm->GetOutput()->GetLabelObject(255)->GetPerimeter() << "\t" 
                << mass->GetSurfaceArea() << "\t" 
                << mass2->GetSurfaceArea() << "\t" 
                << isurf << "\t" 
                << std::abs(bi2lm->GetOutput()->GetLabelObject(255)->GetPerimeter() - mass->GetSurfaceArea()) / std::max(bi2lm->GetOutput()->GetLabelObject(255)->GetPerimeter(), mass->GetSurfaceArea()) * 100 << "\t" 
                << std::abs(bi2lm->GetOutput()->GetLabelObject(255)->GetPerimeter() - mass2->GetSurfaceArea()) / std::max(bi2lm->GetOutput()->GetLabelObject(255)->GetPerimeter(), mass2->GetSurfaceArea()) * 100 << "\t" 
                << std::abs(bi2lm->GetOutput()->GetLabelObject(255)->GetPerimeter() - isurf) / std::max(bi2lm->GetOutput()->GetLabelObject(255)->GetPerimeter(), isurf) * 100 << "\t" 
                << argv[fi] << "\t" 
                << std::endl; 
    continue;
  //  return EXIT_SUCCESS;
    
    
    
    vtkSmartPointer<vtkRenderer> renderer = 
        vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(.1, .2, .3);
  
    vtkSmartPointer<vtkRenderWindow> renderWindow = 
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = 
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);
  
    vtkSmartPointer<vtkPolyDataMapper> mapper = 
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(surface->GetOutputPort());
  
    vtkSmartPointer<vtkActor> actor = 
        vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
  
    renderer->AddActor(actor);
  
    renderWindow->Render();
    interactor->Start();
    }
  return EXIT_SUCCESS;
}
