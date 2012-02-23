#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"

#include "itkLabelPerimeterEstimationCalculator.h"


int main(int argc, char * argv[])
{

  if( argc != 2 )
    {
    std::cerr << "usage: " << argv[0] << " " << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

#if defined(DIM)
  const int dim = DIM;
#else
  const int dim = 3;
#endif
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  std::cout << "spacing: " << reader->GetOutput()->GetSpacing() << std::endl;

  typedef itk::LabelPerimeterEstimationCalculator< IType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetImage( reader->GetOutput() );

  // itk::SimpleFilterWatcher watcher(filter, "filter");

  filter->Compute();
  for(int i=0; i<=itk::NumericTraits<PType>::max(); i++)
    {
    if( filter->HasLabel(i) )
      {
      std::cout << i << ": " << filter->GetPerimeter(i) << std::endl;
      }
    }
  return 0;
}

